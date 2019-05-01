#include "writeOutput.h"

#include <unistd.h>
#include <stdlib.h>


void UpdateMinerCurrentCount(MinerInfo *minerInfo, unsigned int current_count) {
    minerInfo->current_count = current_count;
}

void UpdateSmelterCounts(SmelterInfo *smelterInfo, unsigned int waiting_ore_count, unsigned int produced_ingot_count){
  smelterInfo->waiting_ore_count = waiting_ore_count;
  smelterInfo->total_produce = produced_ingot_count;
}

void UpdateFoundryCounts(FoundryInfo *foundryInfo, unsigned int waiting_iron, unsigned int waiting_coal, unsigned int produced_ingot_count){
  foundryInfo->waiting_iron = waiting_iron;
  foundryInfo->waiting_coal = waiting_coal;
  foundryInfo->total_produce = produced_ingot_count;
}

void UpdateTransporterOre(TransporterInfo *transporterInfo,OreType *carry){
  transporterInfo->carry = carry;
}

typedef struct MinerArgs {
    MinerInfo* minerInfo;
    unsigned int mine_resource;
    unsigned int miner_time;
} MinerArgs;

typedef struct TransporterArgs {
  TransporterInfo* transporterInfo;
  MinerInfo** miners;
  SmelterInfo** smelters;
  FoundryInfo** foundries;
  unsigned int* miner_status;
  unsigned int transport_time;
  unsigned int number_of_mine;
  unsigned int number_of_smelters;
  unsigned int number_of_foundries;
} TransporterArgs;

typedef struct SmelterArgs {
  SmelterInfo* smelterInfo;
  unsigned int smelter_time;
} SmelterArgs;

typedef struct FoundryArgs {
  FoundryInfo* foundryInfo;
  unsigned int foundry_time;

} FoundryArgs;

typedef struct SmelterWaitingArgs {
  SmelterInfo** smelters;
  unsigned int number_of_smelters;
  OreType* oreType;
  unsigned int* index;
} SmelterWaitingArgs;

typedef struct FoundryWaitingArgs {
  FoundryInfo** foundries;
  unsigned int number_of_foundries;
  OreType* oreType;
  unsigned int* index;
} FoundryWaitingArgs;
