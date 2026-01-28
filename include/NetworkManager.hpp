/**
 * @file NetworkManager.hpp
 * @brief Manages the networking layer using ENet.
 */

#pragma once
#include "game.hpp"
#include <string>

// Forward declarations for ENet structures to avoid including enet.h in the header
struct _ENetHost;
typedef struct _ENetHost ENetHost;
struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;

// Enumeration of all packet types used for communication
enum PacketType { 
    PACKET_INPUT,       // Player input state
    PACKET_SEED,        // RNG Seeds for synchronization
    PACKET_TICK,        // Gravity tick (Server to Client)
    PACKET_RESTART_REQ, // Request to restart
    PACKET_RESTART_RES, // Response to restart
    PACKET_PAUSE_REQ,   // Request to pause
    PACKET_PAUSE_RES,   // Response to pause
    PACKET_RESUME_REQ,  // Request to resume
    PACKET_RESUME_RES,  // Response to resume
    PACKET_QUIT,        // Player disconnected/quit
    PACKET_NEW_GAME     // Force new game sync
};

// Defines the network role of the application instance
enum NetworkRole { NONE, SERVER, CLIENT };

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    // Initializes an ENet host as a Server on the specified port.
    bool StartServer(int port);

    // Initializes an ENet host as a Client and connects to a server.
    bool StartClient(const char* hostName, int port);
    
    // Main network loop. Polls ENet events and updates game state accordingly.
    void Update(Game& localGame, Game& remoteGame, bool& isPausedGame, float& countdownTimer, bool useSameSeeds = true);
    
    // Disconnects peers and destroys the host.
    void Stop();

    // Data Transmission Methods ---
    void SendInput(InputState input);
    void SendSeed(unsigned int seedHost, unsigned int seedClient);
    void SendTick();
    void SendRequest(PacketType type);
    void SendResponse(PacketType type, bool accepted);
    void SendQuit();
    void SendNewGameSignal(); 

    // --- State Flags ---
    bool restartRequestReceived = false;
    bool restartRequestPending = false;
    bool pauseRequestReceived = false;
    bool pauseRequestPending = false;
    bool resumeRequestReceived = false;
    bool resumeRequestPending = false;
    bool opponentQuit = false;
    bool remoteStartedNewGame = false; 

    // Retrieves the local machine's IP address (prioritizes private network IPs).
    std::string GetLocalIPInfo(); 

    NetworkRole role;
    bool isConnected; 

private:
    ENetHost* host;
    ENetPeer* peer;
};