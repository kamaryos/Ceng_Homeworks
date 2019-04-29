#include "writeOutput.h"
#include <unistd.h>
#include <stdlib.h>

#include <time.h>

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

void main_miner_routine(MinerInfo* minerInfo,unsigned int mine_resource, unsigned int miner_time){
  //This two line may be differ since I initialize the current_ore_count before sending here
  unsigned int current_ore_count = 0;
  const unsigned int I_m = miner_time;
  UpdateMinerCurrentCount(minerInfo,current_ore_count); // Update the miner with 0 current count
  WriteOutput(minerInfo,NULL,NULL,NULL,MINER_CREATED);
  while(mine_resource > 0){ // Available resources for the mine
    if(minerInfo->capacity > 0){ //Capacity that the miner have is available
      //WaitCanProduce();
      UpdateMinerCurrentCount(minerInfo,current_ore_count);
      WriteOutput(minerInfo,NULL,NULL,NULL,MINER_STARTED);

      current_ore_count += 1;
      UpdateMinerCurrentCount(minerInfo,current_ore_count);
      WriteOutput(minerInfo,NULL,NULL,NULL,MINER_FINISHED);
      //Sleep in interval using I_m
    }
    else{ //Wait until a storage is cleared

    }
  }
  UpdateMinerCurrentCount(minerInfo,current_ore_count); // Update the miner with 0 current count
  WriteOutput(minerInfo,NULL,NULL,NULL,MINER_STOPPED);
}



void main_smelter_routine(SmelterInfo* smelterInfo, unsigned int smelter_time){

  unsigned int waiting_ore_count = 0x02;
  unsigned int produced_ingot_count = 0;
  const unsigned int I_m = smelter_time;
  UpdateSmelterCounts(smelterInfo,waiting_ore_count,produced_ingot_count);
  WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STARTED);
  clock_t start,end;
  start = clock();
  while(1){
    if(smelterInfo->waiting_ore_count == 0){ // ores are completed!
      waiting_ore_count -= 2;
      UpdateSmelterCounts(smelterInfo,waiting_ore_count,produced_ingot_count);
      WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STARTED);
      // Do the work and sleep with I_m
      produced_ingot_count += 1 ;
      //Signals transports for two space is cleared
      UpdateSmelterCounts(smelterInfo,waiting_ore_count,produced_ingot_count);
      WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_FINISHED);
    }
    else{
      end = clock();
      if(((double)t)/CLOCKS_PER_SEC > 4.99 ) break;
      //continue to wait
    }
  }
  //Marks that the smelter is stopped so transporter will not carry anything to this
  UpdateSmelterCounts(smelterInfo,waiting_ore_count,produced_ingot_count);
  WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STOPPED);

}

void main_foundry_routine(FoundryInfo* foundryInfo, unsigned int foundry_time){

  unsigned int waiting_coal = 0x01;
  unsigned int waiting_iron = 0x01;
  unsigned int produced_ingot_count = 0;
  const unsigned int I_m = foundry_time;
  UpdateFoundryCounts(foundryInfo,waiting_coal,waiting_iron,produced_ingot_count);
  WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_CREATED);
  clock_t start,end;
  start = clock();
  while(1){
    if(smelterInfo->waiting_iron == 0 && smelterInfo->waiting_coal == 0 ){
      waiting_iron -= 1;
      waiting_coal -= 1;
      UpdateFoundryCounts(foundryInfo,waiting_iron,waiting_coal,produced_ingot_count);
      WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_STARTED);
      //Sleep a value in range of I_m
      produced_ingot_count += 1 ;
      // signal that foundry is produced
      UpdateFoundryCounts(foundryInfo,waiting_iron,waiting_coal,produced_ingot_count);
      WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_FINISHED);
    }
    else{
      end = clock();
      if(((double)t)/CLOCKS_PER_SEC > 4.99 ) break;
    }
  }
  //Foundry stopped signal!!
  UpdateFoundryCounts(foundryInfo,waiting_iron,waiting_coal,produced_ingot_count);
  WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_STOPPED);
}

void trasnporter_main_routine(TransporterInfo* transporterInfo,unsigned int transport_times,MinerInfo** miners,
  SmelterInfo** smelters,FoundryInfo** foundries, unsigned int number_of_mine){


    OreType *carriedOre = NULL;
    UpdateTransporterOre(transporterInfo,carriedOre);
    WriteOutput(NULL,transporterInfo,NULL,NULL,TRANSPORTER_CREATED);


    while(){ // check whether there are active miners or have mine in their storage
      carriedOre = NULL;
      //Wait miners next load
      //Transporter miner routine
      //Wait producer
      //Do the routine according to it
    }

    FillTransporterInfo(transporterInfo,carriedOre);
    WriteOutput(NULL,transporterInfo,NULL,NULL,TRANSPORTER_STOPPED);
}

void transporter_miner_routine(MinerInfo* minerInfo, TransporterInfo* transpoterInfo,unsigned int transport_times ){

  OreType *carriedOre = NULL;
  const unsigned int I_m = transport_time;
  WriteOutput(minerInfo,transporterInfo,NULL,NULL,TRANSPORTER_TRAVEL);
  //Sleep in a range I_m
  minerInfo->current_count -= 1;
  carriedOre = minerInfo->oreType;
  UpdateMinerCurrentCount(miner,minerInfo->current_count);
  UpdateTransporterOre(transporterInfo,carriedOre);
  WriteOutput(minerInfo,transporterInfo,NULL,NULL,TRANSPORTER_TAKE_ORE);
  //Sleep in a range I_m
  //Signal the miner that new storage is available

}

void transporter_smelter_routine(SmelterInfo* smelterInfo,TransporterInfo* transpoterInfo, unsigned int transport_time){

  OreType *carriedOre =transporterInfo->carriedOre;
  const unsigned int I_m = transport_time;
  WriteOutput(NULL,transporterInfo,smelterInfo,NULL,TRANSPORTER_TRAVEL);
  //Sleep a value in range main_miner_routine
  smelterInfo->waiting_ore_count += 1;
  UpdateSmelterCounts(smelterInfo,smelterInfo->waiting_ore_count,smelterInfo->total_produce);
  UpdateTransporterOre(transpoterInfo,carriedOre);
  WriteOutput(NULL,transpoterInfo,smelterInfo,NULL,TRANSPORTER_DROP_ORE);
  //Sleep in a range I_m
  //Signal that is dropped to its storage

}

void transporter_foundry_routine(FoundryInfo* foundryInfo,TransporterInfo* transpoterInfo, unsigned int transporter_time){

  OreType *carriedOre =transporterInfo->carriedOre;
  const unsigned int I_m = transport_time;
  WriteOutput(NULL,transporterInfo,NULL,foundryInfo,TRANSPORTER_TRAVEL);
  //Sleep a value in range main_miner_routine
  if(carriedOre == IRON) foundryInfo->waiting_iron += 1;
  else foundryInfo->waiting_coal += 1;
  UpdateFoundryCounts(foundryInfo,foundryInfo->waiting_iron,foundryInfo->waiting_coal,foundryInfo->produced_ingot_count);
  UpdateTransporterOre(transpoterInfo,carriedOre);
  WriteOutput(NULL,transporterInfo,NULL,foundryInfo,TRANSPORTER_DROP_ORE);
  //Sleep in a range I_m
  //Signal that is dropped to its storage

}
