/**
 * @file NetworkManager.cpp
 * @brief Implementation of the NetworkManager class.
 * Handles server/client creation, packet transmission, and state synchronization.
 */

// 1. Windows-specific protection definitions
#if defined(_WIN32)
    #define NOGDI             
    #define NOUSER            
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif

#include <enet/enet.h>
#include "../include/NetworkManager.hpp"
#include "raylib.h" 
#include "../include/utils.hpp" 
#include <iostream>
#include <ctime>
#include <string>

// --- LOCAL HELPER FUNCTIONS ---

// Resets synchronization state variables.
// Ensures the game starts smoothly by unpausing, setting the countdown,
// and resetting the time accumulator in Utils.
static void ResetSyncState(bool& isPausedGame, float& countdownTimer) {
    isPausedGame = false;
    countdownTimer = 3.5f;
    Utils::EventTriggered(0, true); // Force reset of lastUpdateTime in Utils
}

// --- CONSTRUCTOR / DESTRUCTOR ---

NetworkManager::NetworkManager() 
    : restartRequestReceived(false), restartRequestPending(false),
      pauseRequestReceived(false), pauseRequestPending(false),
      resumeRequestReceived(false), resumeRequestPending(false),
      opponentQuit(false), remoteStartedNewGame(false),
      role(NONE), isConnected(false), 
      host(nullptr), peer(nullptr) 
{
    if (enet_initialize() != 0) {
        std::cerr << "[Network] Error initializing ENet!\n";
    }
}

NetworkManager::~NetworkManager() {
    Stop();
    enet_deinitialize();
}

// --- CONNECTION MANAGEMENT ---

bool NetworkManager::StartServer(int port) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = (enet_uint16)port;
    
    // Create host allowing 32 connections, 2 channels, no bandwidth limits
    host = enet_host_create(&address, 32, 2, 0, 0);
    
    if (host) { 
        role = SERVER; 
        opponentQuit = false;
        return true; 
    }
    return false;
}

bool NetworkManager::StartClient(const char* hostName, int port) {
    if (host) {
        enet_host_destroy(host);
        host = nullptr;
    }

    // Create client host (1 connection allowed)
    host = enet_host_create(NULL, 1, 2, 0, 0);
    if (!host) return false;

    ENetAddress address;
    enet_address_set_host(&address, hostName);
    address.port = (enet_uint16)port;
    
    // Initiate connection
    peer = enet_host_connect(host, &address, 2, 0);
    
    if (peer) { 
        role = CLIENT; 
        opponentQuit = false; 
        return true; 
    }
    return false;
}

// --- DATA SENDING METHODS ---

void NetworkManager::SendInput(InputState input) {
    if (!peer) return;
    struct { PacketType type; InputState data; } p = { PACKET_INPUT, input };
    ENetPacket* packet = enet_packet_create(&p, sizeof(p), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void NetworkManager::SendSeed(unsigned int seedHost, unsigned int seedClient) {
    if (!peer) return;
    struct { PacketType type; unsigned int s1; unsigned int s2; } p = { PACKET_SEED, seedHost, seedClient };
    ENetPacket* packet = enet_packet_create(&p, sizeof(p), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void NetworkManager::SendTick() {
    if (!peer || role != SERVER) return;
    PacketType type = PACKET_TICK;
    ENetPacket* packet = enet_packet_create(&type, sizeof(type), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void NetworkManager::SendRequest(PacketType type) {
    if (!peer) return;
    ENetPacket* packet = enet_packet_create(&type, sizeof(type), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    
    // Set pending flags based on request type
    if (type == PACKET_RESTART_REQ) restartRequestPending = true;
    if (type == PACKET_PAUSE_REQ) pauseRequestPending = true;
    if (type == PACKET_RESUME_REQ) resumeRequestPending = true;
}

void NetworkManager::SendResponse(PacketType type, bool accepted) {
    if (!peer) return;
    struct { PacketType t; bool a; } p = { type, accepted };
    ENetPacket* packet = enet_packet_create(&p, sizeof(p), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void NetworkManager::SendQuit() {
    if (!peer) return;
    PacketType type = PACKET_QUIT;
    ENetPacket* packet = enet_packet_create(&type, sizeof(type), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(host); // Force send immediately
}

void NetworkManager::SendNewGameSignal() {
    if (!peer) return;
    PacketType type = PACKET_NEW_GAME;
    ENetPacket* packet = enet_packet_create(&type, sizeof(type), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

// --- MAIN UPDATE LOOP ---

void NetworkManager::Update(Game& localGame, Game& remoteGame, bool& isPausedGame, float& countdownTimer, bool useSameSeeds) {
    if (!host) return;
    ENetEvent event;

    // Poll ENet for events
    while (enet_host_service(host, &event, 0) > 0) {
        
        // 1. CONNECTION ESTABLISHED
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            peer = event.peer;
            isConnected = true;
            opponentQuit = false;
            
            // If Server: Determine Seeds and Start Game
            if (role == SERVER) {
                unsigned int s1 = (unsigned int)time(NULL); 
                unsigned int s2 = useSameSeeds ? s1 : s1 + 9999; 
                
                SendSeed(s1, s2);
                localGame.Reset(s1);  
                remoteGame.Reset(s2); 
                
                ResetSyncState(isPausedGame, countdownTimer); 
            }
        } 
        
        // 2. PACKET RECEIVED
        else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            PacketType* type = (PacketType*)event.packet->data;

            switch (*type) {
                case PACKET_INPUT: {
                    // Apply opponent's input and update their score/level representation
                    struct { PacketType t; InputState d; }* pi = (decltype(pi))event.packet->data;
                    remoteGame.HandleInput(pi->d);
                    remoteGame.score = pi->d.currentScore; 
                    remoteGame.level = 1 + (remoteGame.totalLinesCleared / 10);
                    break;
                }
                case PACKET_SEED: { 
                    // Client: Receive initial seeds from Server
                    struct { PacketType t; unsigned int s1; unsigned int s2; }* ps = (decltype(ps))event.packet->data;
                    localGame.Reset(ps->s2);  
                    remoteGame.Reset(ps->s1); 
                    
                    ResetSyncState(isPausedGame, countdownTimer);
                    break;
                }
                case PACKET_TICK: {
                    // Client: Apply gravity signal from Server
                    if (role == CLIENT && countdownTimer <= 0) {
                        localGame.MoveBlockDown();
                        remoteGame.MoveBlockDown();
                    }
                    break;
                }
                
                // Simple Requests
                case PACKET_RESTART_REQ: restartRequestReceived = true; break;
                case PACKET_PAUSE_REQ:   pauseRequestReceived = true;   break;
                case PACKET_RESUME_REQ:  resumeRequestReceived = true;  break;
                case PACKET_QUIT:        opponentQuit = true;           break;
                case PACKET_NEW_GAME:    remoteStartedNewGame = true;   break;

                // Responses
                case PACKET_RESTART_RES: {
                    struct { PacketType t; bool a; }* res = (decltype(res))event.packet->data;
                    restartRequestPending = false;
                    if (res->a) {
                        // If accepted, Server generates new seeds
                        if (role == SERVER) {
                            unsigned int s1 = (unsigned int)time(NULL);
                            unsigned int s2 = useSameSeeds ? s1 : s1 + 9999;
                            SendSeed(s1, s2); 
                            localGame.Reset(s1); remoteGame.Reset(s2);
                        } else { 
                            localGame.Reset(); remoteGame.Reset(); 
                        }
                        ResetSyncState(isPausedGame, countdownTimer); 
                    }
                    break;
                }
                case PACKET_PAUSE_RES: {
                    struct { PacketType t; bool a; }* res = (decltype(res))event.packet->data;
                    pauseRequestPending = false;
                    if (res->a) isPausedGame = true; 
                    break;
                }
                case PACKET_RESUME_RES: {
                    struct { PacketType t; bool a; }* res = (decltype(res))event.packet->data;
                    resumeRequestPending = false;
                    if (res->a) {
                        ResetSyncState(isPausedGame, countdownTimer); 
                    }
                    break;
                }
                default: break;
            }
            enet_packet_destroy(event.packet);
        } 
        
        // 3. DISCONNECTION
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            isConnected = false;
            peer = nullptr;
            if (role == SERVER) opponentQuit = true; 
        }
    }
}

void NetworkManager::Stop() {
    if (peer) {
        enet_peer_disconnect(peer, 0);
        enet_host_flush(host);
    }
    if (host) enet_host_destroy(host);
    
    host = nullptr; peer = nullptr; isConnected = false; role = NONE;
    
    //Reset all state flags
    restartRequestReceived = false; restartRequestPending = false;
    pauseRequestReceived = false; pauseRequestPending = false;
    resumeRequestReceived = false; resumeRequestPending = false;
    opponentQuit = false; remoteStartedNewGame = false;
}

std::string NetworkManager::GetLocalIPInfo() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct hostent* hostInfo = gethostbyname(hostname);
        if (hostInfo && hostInfo->h_addr_list[0]) {
            std::string fallbackIP = "";
            for (int i = 0; hostInfo->h_addr_list[i] != NULL; i++) {
                struct in_addr addr;
                addr.s_addr = *(u_long*)hostInfo->h_addr_list[i];
                std::string ipStr = inet_ntoa(addr);

                // Filter out loopback (127.x) and common virtual adapters (26.x)
                if (ipStr == "127.0.0.1" || ipStr.substr(0, 3) == "26.") continue;

                // Prioritize private network classes (A, B, C)
                bool isPrivate = (ipStr.substr(0, 3) == "10.") || 
                                 (ipStr.substr(0, 8) == "192.168") ||
                                 (ipStr.substr(0, 4) == "172.");

                if (isPrivate) return "IP: " + ipStr + " | PC: " + std::string(hostname);
                if (fallbackIP == "") fallbackIP = ipStr;
            }
            return "IP: " + (fallbackIP != "" ? fallbackIP : "127.0.0.1") + " | PC: " + std::string(hostname);
        }
    }
    return "IP: 127.0.0.1";
}