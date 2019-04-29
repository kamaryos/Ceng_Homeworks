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
