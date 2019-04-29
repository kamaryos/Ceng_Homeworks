#include "tuncay.h"

#include <time.h>
#include <pthread.h>
#include <semaphore.h>



void main_miner_routine(MinerInfo* minerInfo,unsigned int mine_resource, unsigned int miner_time){
  //This two line may be differ since I initialize the current_ore_count before sending here
  unsigned int current_ore_count = 0;
  //UpdateMinerCurrentCount(minerInfo,current_ore_count); // Update the miner with 0 current count
  WriteOutput(minerInfo,NULL,NULL,NULL,MINER_CREATED);
  const unsigned int I_m = miner_time;
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
  //UpdateSmelterCounts(smelterInfo,waiting_ore_count,produced_ingot_count);
  WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STARTED);
  clock_t start,end,t;
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
      t = end-start;
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
  //UpdateFoundryCounts(foundryInfo,waiting_coal,waiting_iron,produced_ingot_count);
  WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_CREATED);
  clock_t start,end,t;
  start = clock();
  while(1){
    if(foundryInfo->waiting_iron == 0 && foundryInfo->waiting_coal == 0 ){
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
      t = end-start;
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
    //UpdateTransporterOre(transporterInfo,carriedOre);
    WriteOutput(NULL,transporterInfo,NULL,NULL,TRANSPORTER_CREATED);


    while(1){ // check whether there are active miners or have mine in their storage
      carriedOre = NULL;
      //Wait miners next load
      //Transporter miner routine
      //Wait producer
      //Do the routine according to it
    }

    UpdateTransporterOre(transporterInfo,carriedOre);
    WriteOutput(NULL,transporterInfo,NULL,NULL,TRANSPORTER_STOPPED);
}

void transporter_miner_routine(MinerInfo* minerInfo, TransporterInfo* transporterInfo,unsigned int transport_time){

  OreType *carriedOre = NULL;
  const unsigned int I_m = transport_time;
  WriteOutput(minerInfo,transporterInfo,NULL,NULL,TRANSPORTER_TRAVEL);
  //Sleep in a range I_m
  minerInfo->current_count -= 1;
  carriedOre = &(minerInfo->oreType);
  UpdateMinerCurrentCount(minerInfo,minerInfo->current_count);
  UpdateTransporterOre(transporterInfo,carriedOre);
  WriteOutput(minerInfo,transporterInfo,NULL,NULL,TRANSPORTER_TAKE_ORE);
  //Sleep in a range I_m
  //Signal the miner that new storage is available

}

void transporter_smelter_routine(SmelterInfo* smelterInfo,TransporterInfo* transporterInfo, unsigned int transport_time){

  OreType *carriedOre = transporterInfo->carry;
  const unsigned int I_m = transport_time;
  WriteOutput(NULL,transporterInfo,smelterInfo,NULL,TRANSPORTER_TRAVEL);
  //Sleep a value in range main_miner_routine
  smelterInfo->waiting_ore_count += 1;
  UpdateSmelterCounts(smelterInfo,smelterInfo->waiting_ore_count,smelterInfo->total_produce);
  UpdateTransporterOre(transporterInfo,carriedOre);
  WriteOutput(NULL,transporterInfo,smelterInfo,NULL,TRANSPORTER_DROP_ORE);
  //Sleep in a range I_m
  //Signal that is dropped to its storage

}

void transporter_foundry_routine(FoundryInfo* foundryInfo,TransporterInfo* transporterInfo, unsigned int transporter_time){

  OreType *carriedOre = transporterInfo->carry;
  const unsigned int I_m = transporter_time;
  WriteOutput(NULL,transporterInfo,NULL,foundryInfo,TRANSPORTER_TRAVEL);
  //Sleep a value in range main_miner_routine
  if(carriedOre == IRON) foundryInfo->waiting_iron += 1;
  else foundryInfo->waiting_coal += 1;
  UpdateFoundryCounts(foundryInfo,foundryInfo->waiting_iron,foundryInfo->waiting_coal,foundryInfo->total_produce);
  UpdateTransporterOre(transporterInfo,carriedOre);
  WriteOutput(NULL,transporterInfo,NULL,foundryInfo,TRANSPORTER_DROP_ORE);
  //Sleep in a range I_m
  //Signal that is dropped to its storage

}



int main(int argc, char *argv[]){


  // Waiting count may change in reverse order


  // Reading the inputs from the standart input and filling the buffers according to it.
  char buf[10];
  read(0,buf,2);

  // Miner and mine resource filling
  unsigned int number_of_mine = strtoul(buf,NULL,3);
  printf("%u \n",number_of_mine);

  MinerInfo* miners[number_of_mine];
  unsigned int miner_times[number_of_mine];
  unsigned int mine_resource[number_of_mine];


  for(int i = 0 ; i < number_of_mine ; i++){
    unsigned int ID = i+1;
    read(0,buf,2);
    miner_times[i] = strtoul(buf,NULL,0);
    read(0,buf,2);
    unsigned int capacity = strtoul(buf,NULL,0);
    read(0,buf,2);
    unsigned int oreIndex  = strtoul(buf,NULL,0);
    OreType oreType = oreIndex;
    miners[i] = (MinerInfo*)malloc(sizeof(MinerInfo));
    FillMinerInfo(miners[i],ID,oreType,capacity,0x00);
    read(0,buf,2);
    mine_resource[i] = strtoul(buf,NULL,0);
  }

  //Transporter information is added to the buffers.
  read(0,buf,2);
  unsigned int number_of_transporters = strtoul(buf,NULL,0);

  TransporterInfo* transporters[number_of_transporters];
  unsigned int transport_times[number_of_transporters];

  for(int i = 0 ; i < number_of_transporters ; i++){
    unsigned int ID = i+1;
    read(0,buf,2);
    transport_times[i] = strtoul(buf,NULL,0);
    transporters[i] = (TransporterInfo*)malloc(sizeof(TransporterInfo));
    FillTransporterInfo(transporters[i],ID,NULL);
  }

  //Smelter information is added to the buffers.
  read(0,buf,2);
  unsigned int number_of_smelters = strtoul(buf,NULL,0);

  SmelterInfo* smelters[number_of_smelters];
  unsigned int smelter_times[number_of_smelters];

  for(int i = 0 ; i < number_of_smelters ; i++){
    unsigned int ID = i+1;
    read(0,buf,2);
    smelter_times[i] = strtoul(buf,NULL,0);
    read(0,buf,2);
    unsigned int capacity = strtoul(buf,NULL,0);
    read(0,buf,2);
    unsigned int oreIndex  = strtoul(buf,NULL,0);
    OreType oreType = oreIndex;
    smelters[i] = (SmelterInfo*)malloc(sizeof(SmelterInfo));
    FillSmelterInfo(smelters[i],ID,oreType,capacity,0,0);
  }

  //Foundries information is added to the buffers.
  read(0,buf,2);
  unsigned int number_of_foundries = strtoul(buf,NULL,0);

  FoundryInfo* foundries[number_of_foundries];
  unsigned int foundry_times[number_of_foundries];

  for(int i = 0 ; i < number_of_foundries ; i++){
    unsigned int ID = i+1;
    read(0,buf,2);
    foundry_times[i] = strtoul(buf,NULL,0);
    read(0,buf,2);
    unsigned int capacity = strtoul(buf,NULL,0);
    foundries[i] = (FoundryInfo*)malloc(sizeof(FoundryInfo));
    FillFoundryInfo(foundries[i],ID,capacity,0,0,0);
  }

  // Starting from here start the implementation of mining simulation



  return 0;

}
