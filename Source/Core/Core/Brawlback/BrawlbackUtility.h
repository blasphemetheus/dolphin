#pragma once

#include <unordered_map>
#include <array>

#include "Common/CommonTypes.h"
#include "Core/Brawlback/Brawltypes.h"

#include "Common/Logging/Log.h"
#include "Common/Logging/LogManager.h"

#include "SlippiUtility.h"

#define MAX_ROLLBACK_FRAMES 7
#define FRAME_DELAY 2

// number of FrameData's to keep in the queue
#define FRAMEDATA_MAX_QUEUE_SIZE 30 
// update ping display every X frames
#define PING_DISPLAY_INTERVAL 1

#define ONLINE_LOCKSTEP_INTERVAL 30
#define GAME_START_FRAME 1

#define MAX_REMOTE_PLAYERS 3
#define MAX_NUM_PLAYERS 4
#define BRAWLBACK_PORT 7779

// 59.94 Hz   ( -- is this accurate? This is the case for melee, idk if it also applies here)
#define MS_IN_FRAME 16683


namespace Brawlback {
    const u8 NAMETAG_SIZE = 8;
    const u8 DISPLAY_NAME_SIZE = 31;
    const u8 CONNECT_CODE_SIZE = 10;

    enum EXICommand : u8
    {
      CMD_UNKNOWN = 0,


      CMD_ONLINE_INPUTS = 1,
      CMD_CAPTURE_SAVESTATE = 2,
      CMD_LOAD_SAVESTATE = 3,

      CMD_FIND_OPPONENT = 5,
      CMD_START_MATCH = 13,
      CMD_SETUP_PLAYERS = 14,
      CMD_FRAMEDATA = 15,
      CMD_TIMESYNC = 16,
      CMD_ROLLBACK = 17,

      CMD_GET_MATCH_STATE = 4,
      CMD_SET_MATCH_SELECTIONS = 6,

      CMD_OPEN_LOGIN = 7,
      CMD_LOGOUT = 8,
      CMD_UPDATE = 9,
      
      CMD_GET_ONLINE_STATUS = 10,
      CMD_CLEANUP_CONNECTION = 11,
      CMD_GET_NEW_SEED = 12,
    };

    enum NetPacketCommand : u8 
    {
        CMD_FRAME_DATA = 1,
        CMD_GAME_SETTINGS = 2,
        CMD_FRAME_DATA_ACK = 3,
    };

    struct FrameOffsetData {
        int idx;
        std::vector<s32> buf;
    };

    namespace Match
    {

        enum PlayerType : u8
        {
            PLAYERTYPE_LOCAL = 0x0,
            PLAYERTYPE_REMOTE = 0x1,
        };

        
        struct PlayerFrameData {
            u32 frame;
            u8 playerIdx;
            gfPadGamecube pad;

            PlayerFrameData() {
                frame = 0;
                playerIdx = 0;
                pad = gfPadGamecube();
            }
        };

        //#pragma pack(push, 4)
        struct FrameData {
            u32 randomSeed;
            PlayerFrameData playerFrameDatas[MAX_NUM_PLAYERS];
        };
        //#pragma pack(pop)

        struct PlayerSettings
        {
            u8 charID;
            u8 charColor;
            PlayerType playerType;
            u8 controllerPort;
            u16 nametag[NAMETAG_SIZE];
            u8 displayName[DISPLAY_NAME_SIZE];
            u8 connectCode[CONNECT_CODE_SIZE];
        };

        struct GameSettings
        {
            u8 localPlayerIdx;
            u8 numPlayers;
            u16 stageID;
            u32 randomSeed;
            PlayerSettings playerSettings[MAX_NUM_PLAYERS];
        };

        struct Game {
            u32 version;
            GameSettings gameSettings;
            u32 currentFrame;
            std::unordered_map<int32_t, FrameData*> framesByIndex;
            std::vector<std::unique_ptr<FrameData>> frames;
        };

        struct RollbackInfo {
            bool isUsingPredictedInputs;
            u32 beginFrame; // frame we realized we have no remote inputs
            u32 endFrame; // frame we received new remote inputs, and should now resim with those
            PlayerFrameData predictedInputs;

            bool pastFrameDataPopulated;
            FrameData pastFrameDatas[MAX_ROLLBACK_FRAMES];

            bool hasPreserveBlocks;
            std::vector<SlippiUtility::Savestate::PreserveBlock> preserveBlocks;

            RollbackInfo() {
                isUsingPredictedInputs = false;
                beginFrame = 0;
                endFrame = 0;
                predictedInputs = PlayerFrameData();
                //pastFrameDatas
                hasPreserveBlocks = false;
                preserveBlocks = {};
                pastFrameDataPopulated = false;
            }

        };
        
    }




    // checks if the specified `button` is held down based on the buttonBits bitfield
    bool isButtonPressed(u16 buttonBits, PADButtonBits button);

    namespace Mem {
        void print_byte(uint8_t byte);
        void print_half(u16 half);
        void print_word(u32 word);

        template <typename T>
        std::vector<u8> structToByteVector(T* s) {
            auto ptr = reinterpret_cast<u8*>(s);
            return std::vector<u8>(ptr, ptr + sizeof(T));
        }

        void fillByteVectorWithBuffer(std::vector<u8>& vec, u8* buf, size_t size);
    }

}
