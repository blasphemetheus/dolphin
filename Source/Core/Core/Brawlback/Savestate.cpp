#include "Savestate.h"

#include <Core/HW/Memmap.h>
#include "common/Logging/Log.h"
#include <Common/MemoryUtil.h>

BrawlbackSavestate::BrawlbackSavestate()
{
  initBackupLocs();
  for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
  {
    auto size = it->endAddress - it->startAddress;
    it->data = static_cast<u8*>(Common::AllocateAlignedMemory(size, 64));
  }
}

BrawlbackSavestate::~BrawlbackSavestate()
{
  for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
  {
    Common::FreeAlignedMemory(it->data);
  }
}

void BrawlbackSavestate::initBackupLocs()
{

 
  static std::vector<ssBackupLoc> backupRegions = { 
      //{0x805b5160, 0x817da5a0, nullptr}, // all of mem1
      //{0x90000800, 0x935e0000, nullptr}, // all of mem2

      //mem1
      //{0x805b5160, 0x805ca260, nullptr}, // System FW
      //{0x80611f60, 0x80673460, nullptr}, // System
      //{0x80b8db60, 0x80c23a60, nullptr}, // Effect
      //{0x805d1e60, 0x80611f60, nullptr}, // RenderFifo

      //{0x8154e560, 0x81601960, nullptr}, // Physics
      {0x8123ab60, 0x8128cb60, nullptr}, // Fighter1Instance
      //{0x8128cb60, 0x812deb60, nullptr}, // Fighter2Instance
      //{0x80c23a60, 0x80da3a60, nullptr}, // InfoResource
      //{0x80da3a60, 0x80fd6260, nullptr}, // CommonResource
      //{0x815edf60, 0x817bad60, nullptr}, // InfoExtraResource
      //{0x81601960, 0x81734d60, nullptr}, // InfoInstance
      //{0x80673460, 0x80b8db60, nullptr}, // OverlayCommon
      //{0x81061060, 0x810a9560, nullptr}, // OverlayFighter1
      //{0x810a9560, 0x810f1a60, nullptr}, // OverlayFighter2
      //{0x805ca260, 0x805d1e60, nullptr}, // Thread

      // mem2
      //{0x90199800, 0x90e61400, nullptr}, // Sound
      //{0x90e61400, 0x90e77500, nullptr}, // WiiPad
      //{0x91018b00, 0x91301b00, nullptr}, // IteamResource (is this for items, or for teams?)
      //{0x91301b00, 0x9134cc00, nullptr}, // Replay
      //{0x92f34700, 0x9359ae00, nullptr}, // StageResource
      //{0x9151fa00, 0x91a72e00, nullptr}, // Fighter1Resource
      //{0x91b04c80, 0x92058080, nullptr}, // Fighter2Resource
      //{0x91a72e00, 0x91b04c80, nullptr}, // Fighter1Resource2
      //{0x91478e00, 0x914d2900, nullptr}, // Fighter2Resource2
      //{0x92cb4400, 0x92dcdf00, nullptr}, // FighterTechqniq
      //{0x9134cc00, 0x91478e00, nullptr}, // CopyFB
      //{0x90167400, 0x90199800, nullptr}, // GameGlobal
      //{0x90fddc00, 0x91018b00, nullptr}, // GlobalMode // probably don't need to care about this one?
  };

  backupLocs.insert(backupLocs.end(), backupRegions.begin(), backupRegions.end());
}

void BrawlbackSavestate::Capture()
{
  for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
  {
    auto size = it->endAddress - it->startAddress;
    Memory::CopyFromEmu(it->data, it->startAddress, size); // game -> emu
  }
}

void BrawlbackSavestate::Load(std::vector<PreserveBlock> blocks)
{

  // Back up regions of game that should stay the same between savestates
  /*
  for (auto it = blocks.begin(); it != blocks.end(); ++it)
  {
    if (!preservationMap.count(*it)) // if this PreserveBlock is NOT in our preservationMap
    {
      // TODO: Clear preservation map when game ends
      preservationMap[*it] = std::vector<u8>(it->length); // init new entry at this PreserveBlock key
    }

    Memory::CopyFromEmu(&preservationMap[*it][0], it->address, it->length);
  }
  */

  // Restore memory blocks
  for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
  {
    auto size = it->endAddress - it->startAddress;
    Memory::CopyToEmu(it->startAddress, it->data, size); // emu -> game
  }

  // Restore
  /*
  for (auto it = blocks.begin(); it != blocks.end(); ++it)
  {
    Memory::CopyToEmu(it->address, &preservationMap[*it][0], it->length);
  }
  */

}