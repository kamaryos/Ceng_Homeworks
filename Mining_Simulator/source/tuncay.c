#include "tuncay.h"

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
