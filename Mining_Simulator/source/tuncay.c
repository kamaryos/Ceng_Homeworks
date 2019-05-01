#include "tuncay.h"

#include <time.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t minerInfo_locking = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Mutex for protecting critical region

pthread_cond_t cvTransporterMiner = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvMinerTransporter = PTHREAD_COND_INITIALIZER;

pthread_cond_t cvTransporterSmelter = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvSmelterTransporter = PTHREAD_COND_INITIALIZER;

pthread_cond_t cvTransporterFoundry = PTHREAD_COND_INITIALIZER;
pthread_cond_t cvFoundryTransporter = PTHREAD_COND_INITIALIZER;



pthread_mutex_t* minerMutexes;
pthread_mutex_t* smelterMutexes;
pthread_mutex_t* foundryMutexes;

unsigned int check_storage(unsigned int number_of_mine,MinerInfo** minerInfos) {
	for(int i = 0 ; i < number_of_mine; i++)
	{
		if(minerInfos[i]->current_count > 0) return 0x00;
	}
	return 0x01;
}

unsigned int any_active_miners(unsigned int number_of_mine,unsigned int * miner_status){


	for(int i=0; i<number_of_mine; i++)
	{
		if(miner_status[i] == 0x01) return 0x01;
	}
	return 0x00;
}

// Find and return the next miner to go for transporter
MinerInfo* WaitingForNextLoad(unsigned int* kth,unsigned int number_of_mine,MinerInfo** minerInfos) {
	for(int i = *kth; i < number_of_mine; i++)
	{
		if(minerInfos[i]->current_count > 0)
		{
			*kth = i;
			return minerInfos[i];
		}
	}
	for(int i = 0; i < *kth; i++)
	{
		if(minerInfos[i]->current_count > 0)
		{
			*kth = i;
			return minerInfos[i];
		}
	}
	pthread_mutex_lock(&mutex);
	while(check_storage(number_of_mine,minerInfos) == 0x01)
	{
		pthread_cond_wait(&cvTransporterMiner,&mutex);
	}
	return WaitingForNextLoad(kth,number_of_mine,minerInfos);
}

SmelterInfo* FindNextSmelter(void *ptr) // Find and return the next smelter to go for transporter
{
	SmelterWaitingArgs *args = ptr;

	SmelterInfo** smelters = (SmelterInfo**)args->smelters;
	unsigned int number_of_smelters = (unsigned int)args->number_of_smelters;
	OreType* oreType = (OreType*)args->oreType;
	unsigned int* index = (unsigned int*)args->index;
	for(int i = 0; i < number_of_smelters; i++)
	{
		if(smelters[i]->waiting_ore_count == 1 && smelters[i]->oreType == *oreType)
		{
			*index = i;
			return smelters[i];
		}
	}
	for(int i = 0; i < number_of_smelters; i++)
	{
		if(smelters[i]->waiting_ore_count == 0 && smelters[i]->oreType == *oreType)
		{
			*index = i;
			return smelters[i];
		}
	}
	for(int i = 0; i < number_of_smelters; i++)
	{
		if(smelters[i]->waiting_ore_count < smelters[i]->loading_capacity && smelters[i]->oreType == *oreType)
		{
			*index = i;
			return smelters[i];
		}
	}

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cvSmelterTransporter,&mutex);
	args->index = index;
	return FindNextSmelter(args);
}

FoundryInfo* FindNextFoundry(void *ptr){

	FoundryWaitingArgs *args = ptr;

	FoundryInfo** foundries = (FoundryInfo**)args->foundries;
	unsigned int number_of_foundries = (unsigned int)args->number_of_foundries;
	OreType* oreType = (OreType*)args->oreType;
	unsigned int* index = (unsigned int*)args->index;

	for(int i = 0; i < number_of_foundries; i++)
	{
		if(foundries[i]->waiting_iron == 1 && foundries[i]->waiting_coal == 0)
		{
			*index = i;
			return foundries[i];
		}
	}
	for(int i = 0; i < number_of_foundries; i++)
	{
		if(foundries[i]->waiting_iron == 0 && foundries[i]->waiting_coal == 0)
		{
			*index = i;
			return foundries[i];
		}
	}
	// 3) Check for any foundry that is not full.
	for(int i = 0; i < number_of_foundries; i++)
	{
		if((foundries[i]->waiting_iron + foundries[i]->waiting_coal) < foundries[i]->loading_capacity)
		{
			*index = i;
			return foundries[i];
		}
	}

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cvFoundryTransporter,&mutex);
	args->index = index;
	return FindNextFoundry(args);
}

void *main_miner_routine(void *ptr){
  //This two line may be differ since I initialize the current_ore_count before sending here

  MinerArgs* args = ptr;

  MinerInfo* minerInfo = (MinerInfo*)args->minerInfo;
  unsigned int mine_resource = (unsigned int)args->mine_resource;
  const unsigned int I_m = (unsigned int)args->miner_time;


  const unsigned int index = (minerInfo->ID) - 1;
  FillMinerInfo(minerInfo,minerInfo->ID,minerInfo->oreType,minerInfo->capacity,0); // Update the miner with 0 current count
  WriteOutput(minerInfo,NULL,NULL,NULL,MINER_CREATED);
  while(mine_resource > 0){ // Available resources for the mine
    if(minerInfo->current_count == minerInfo->capacity){ //Capacity that the miner have is available
      pthread_mutex_lock(&mutex);
      while(minerInfo->current_count == minerInfo->capacity){
        pthread_cond_wait(&cvTransporterMiner,&mutex);
      }
      pthread_mutex_lock(&minerMutexes[index]);
    }
    //WaitCanProduce();
    FillMinerInfo(minerInfo,minerInfo->ID,minerInfo->oreType,minerInfo->capacity,minerInfo->current_count);
    WriteOutput(minerInfo,NULL,NULL,NULL,MINER_STARTED);
    usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
    pthread_mutex_trylock(&minerMutexes[index]);
    minerInfo->current_count +=1;
    mine_resource -= 1;
    pthread_cond_broadcast(&cvMinerTransporter);
    FillMinerInfo(minerInfo,minerInfo->ID,minerInfo->oreType,minerInfo->capacity,minerInfo->current_count);
    WriteOutput(minerInfo,NULL,NULL,NULL,MINER_FINISHED);
    pthread_mutex_unlock(&minerMutexes[index]);
    //Sleep in interval using I_m
    usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));

  }
  FillMinerInfo(minerInfo,minerInfo->ID,minerInfo->oreType,minerInfo->capacity,minerInfo->current_count); // Update the miner with 0 current count
  WriteOutput(minerInfo,NULL,NULL,NULL,MINER_STOPPED);
}



void *main_smelter_routine(void *ptr){


	SmelterArgs* args = ptr;

	SmelterInfo* smelterInfo = (SmelterInfo*)args->smelterInfo;


	const unsigned int ID = (smelterInfo->ID)-1;

  const unsigned int I_m = (unsigned int)args->smelter_time;

  WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STARTED);
  clock_t start,end,t;
  start = clock();
  while(1){
    if(smelterInfo->waiting_ore_count >= 2){ // ores are completed!
			pthread_mutex_lock(&smelterMutexes[ID]);
			smelterInfo->waiting_ore_count -= 2;
      UpdateSmelterCounts(smelterInfo,smelterInfo->waiting_ore_count,smelterInfo->total_produce);
      WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STARTED);
			usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));

			// Do the work and sleep with I_m
      smelterInfo->total_produce += 1 ;
			pthread_cond_broadcast(&cvSmelterTransporter);
      //Signals transports for two space is cleared
      UpdateSmelterCounts(smelterInfo,smelterInfo->waiting_ore_count,smelterInfo->total_produce);
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
  UpdateSmelterCounts(smelterInfo,smelterInfo->waiting_ore_count,smelterInfo->total_produce);
  WriteOutput(NULL,NULL,smelterInfo,NULL,SMELTER_STOPPED);

}

void *main_foundry_routine(void* ptr){


	FoundryArgs* args = ptr;

	FoundryInfo* foundryInfo = (FoundryInfo*)args->foundryInfo;

	const unsigned int ID = (foundryInfo->ID)-1;
  const unsigned int I_m = (unsigned int)args->foundry_time;
  //UpdateFoundryCounts(foundryInfo,waiting_coal,waiting_iron,produced_ingot_count);
  WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_CREATED);
  clock_t start,end,t;
  start = clock();
  while(1){
    if(foundryInfo->waiting_iron >= 1 && foundryInfo->waiting_coal >= 1 ){
			pthread_mutex_lock(&foundryMutexes[ID]);
			foundryInfo->waiting_iron -= 1;
      foundryInfo->waiting_coal -= 1;
      UpdateFoundryCounts(foundryInfo,foundryInfo->waiting_iron,foundryInfo->waiting_coal,foundryInfo->total_produce);
      WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_STARTED);
			pthread_mutex_unlock(&foundryMutexes[ID]);
			//Sleep a value in range of I_m
			usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
      foundryInfo->total_produce += 1 ;
      // signal that foundry is produced
			pthread_cond_broadcast(&cvFoundryTransporter);
			UpdateFoundryCounts(foundryInfo,foundryInfo->waiting_iron,foundryInfo->waiting_coal,foundryInfo->total_produce);
      WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_FINISHED);
    }
    else{
      end = clock();
      t = end-start;
      if(((double)t)/CLOCKS_PER_SEC > 4.99 ) break;
    }
  }
  //Foundry stopped signal!!
  UpdateFoundryCounts(foundryInfo,foundryInfo->waiting_iron,foundryInfo->waiting_coal,foundryInfo->total_produce);
  WriteOutput(NULL,NULL,NULL,foundryInfo,FOUNDRY_STOPPED);
}

void transporter_miner_routine(MinerInfo* minerInfo, TransporterInfo* transporterInfo,unsigned int transport_time,unsigned int kth){

  MinerInfo* tempMinerInfo = (MinerInfo*)malloc(sizeof(MinerInfo));
  const unsigned int I_m = transport_time;
  FillMinerInfo(tempMinerInfo, minerInfo->ID, IRON, 0, 0);
  FillTransporterInfo(transporterInfo,transporterInfo->ID,transporterInfo->carry);
  WriteOutput(tempMinerInfo,transporterInfo,NULL,NULL,TRANSPORTER_TRAVEL);
  //Sleep in a range I_m
  usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
  pthread_mutex_lock(&minerMutexes[kth]);

  minerInfo->current_count -= 1;
  pthread_mutex_unlock(&minerInfo_locking);
  transporterInfo->carry = &(minerInfo->oreType);
  UpdateMinerCurrentCount(minerInfo,minerInfo->current_count);
  UpdateTransporterOre(transporterInfo,transporterInfo->carry);
  WriteOutput(minerInfo,transporterInfo,NULL,NULL,TRANSPORTER_TAKE_ORE);
  pthread_mutex_unlock(&minerMutexes[kth]);
  //Sleep in a range I_m
  usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
  //Signal the miner that new storage is available
  pthread_cond_broadcast(&cvTransporterMiner);

}



void transporter_smelter_routine(SmelterInfo* smelterInfo,TransporterInfo* transporterInfo, unsigned int transport_time, unsigned int kth){

	SmelterInfo* tempSmelterInfo = (SmelterInfo*)malloc(sizeof(SmelterInfo));
  const unsigned int I_m = transport_time;
	FillSmelterInfo(tempSmelterInfo,smelterInfo->ID,IRON,0,0,0);
	FillTransporterInfo(transporterInfo,transporterInfo->ID,transporterInfo->carry);
  WriteOutput(NULL,transporterInfo,smelterInfo,NULL,TRANSPORTER_TRAVEL);
  //Sleep a value in range I_m
	usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
	pthread_mutex_lock(&smelterMutexes[kth]);
  smelterInfo->waiting_ore_count += 1;
  UpdateSmelterCounts(smelterInfo,smelterInfo->waiting_ore_count,smelterInfo->total_produce);
  UpdateTransporterOre(transporterInfo,transporterInfo->carry);
  WriteOutput(NULL,transporterInfo,smelterInfo,NULL,TRANSPORTER_DROP_ORE);
	pthread_mutex_unlock(&smelterMutexes[kth]);
	usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));

	pthread_cond_broadcast(&cvTransporterSmelter);
	//Sleep in a range I_m
  //Signal that is dropped to its storage

}

void transporter_foundry_routine(FoundryInfo* foundryInfo,TransporterInfo* transporterInfo, unsigned int transporter_time, unsigned int kth){

  const unsigned int I_m = transporter_time;
	FoundryInfo* tempFoundryInfo = (FoundryInfo*)malloc(sizeof(FoundryInfo));
	FillFoundryInfo(tempFoundryInfo,foundryInfo->ID,0,0,0,0);
	FillTransporterInfo(transporterInfo,transporterInfo->ID,transporterInfo->carry);
  WriteOutput(NULL,transporterInfo,NULL,foundryInfo,TRANSPORTER_TRAVEL);
  //Sleep a value in range main_miner_routine
	usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
	pthread_mutex_lock(&foundryMutexes[kth]);
  if(ToString(*(transporterInfo->carry) == IRON)) foundryInfo->waiting_iron += 1;
  else foundryInfo->waiting_coal += 1;
  UpdateFoundryCounts(foundryInfo,foundryInfo->waiting_iron,foundryInfo->waiting_coal,foundryInfo->total_produce);
  UpdateTransporterOre(transporterInfo,transporterInfo->carry);
  WriteOutput(NULL,transporterInfo,NULL,foundryInfo,TRANSPORTER_DROP_ORE);
	pthread_mutex_unlock(&foundryMutexes[kth]);
  //Sleep in a range I_m
	usleep(I_m - (I_m*0.01) + (rand()%(unsigned int)(I_m*0.02)));
  //Signal that is dropped to its storage
	pthread_cond_broadcast(&cvTransporterFoundry);

}


void *main_transporter_routine(void *ptr){

    TransporterArgs* args = ptr;

    TransporterInfo* transporterInfo = (TransporterInfo*)args->transporterInfo;
    MinerInfo** miners = (MinerInfo**)args->miners;
    SmelterInfo** smelters = (SmelterInfo**)args->smelters;
    FoundryInfo** foundries = (FoundryInfo**)args->foundries;
		unsigned int number_of_mine = (unsigned int)args->number_of_mine;
		unsigned int number_of_smelters = (unsigned int)args->number_of_smelters;
		unsigned int number_of_foundries = (unsigned int)args->number_of_foundries;

    const unsigned int I_m = (unsigned int)args->transport_time;

    unsigned int* miner_status = (unsigned int*)args->miner_status;

    unsigned int kth = 0x01;

    FillTransporterInfo(transporterInfo,transporterInfo->ID,transporterInfo->carry);
    WriteOutput(NULL,transporterInfo,NULL,NULL,TRANSPORTER_CREATED);


    while((check_storage(number_of_mine,miners) == 0x01)
  || (any_active_miners(number_of_mine,miner_status) == 0x01)){ // check whether there are active miners or have mine in their storage
      transporterInfo->carry = NULL;
      if(kth > number_of_mine) kth = 0;
      pthread_mutex_lock(&minerInfo_locking);

      //Wait miners next load
			MinerInfo *minerInfo = WaitingForNextLoad(&kth,number_of_mine,miners);
			if(minerInfo == NULL){
        pthread_mutex_unlock(&minerInfo_locking);
        break;
      }
      //Transporter miner routine
      transporter_miner_routine(minerInfo,transporterInfo,I_m,kth);
      //Wait producer
      if(ToString(*(transporterInfo->carry)) == "Copper"){

        //Wait Next Smelter
        //transporter_smelter_routine
				unsigned int index = 0;
				SmelterWaitingArgs arg_smelter;
				arg_smelter.smelters = smelters;
				arg_smelter.number_of_smelters = number_of_smelters;
				arg_smelter.oreType = transporterInfo->carry;
				arg_smelter.index = &index;
				SmelterInfo* smelter_next = FindNextSmelter(&arg_smelter);
				transporter_smelter_routine(smelter_next,transporterInfo,I_m,index);
      }
      else if(ToString(*(transporterInfo->carry)) == "Coals"){
        //Wait next foundry
        //transporter_foundry_routine
				unsigned int index = 0;
				FoundryWaitingArgs arg_foundry;
				arg_foundry.foundries = foundries;
				arg_foundry.number_of_foundries = number_of_foundries;
				arg_foundry.oreType = transporterInfo->carry;
				arg_foundry.index = &index;
				FoundryInfo* foundry_next = FindNextFoundry(&arg_foundry);
      }
      else if(ToString(*(transporterInfo->carry)) == "Iron"){

      }
      //Do the routine according to it
    }

    UpdateTransporterOre(transporterInfo,transporterInfo->carry);
    WriteOutput(NULL,transporterInfo,NULL,NULL,TRANSPORTER_STOPPED);
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
  unsigned int miner_status[number_of_mine];
  minerMutexes = (pthread_mutex_t*)malloc(number_of_mine*sizeof(pthread_mutex_t));

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
  unsigned int transporter_status[number_of_transporters];

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
  unsigned int smelter_status[number_of_smelters];
	smelterMutexes = (pthread_mutex_t*)malloc(number_of_smelters*sizeof(pthread_mutex_t));

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
  unsigned int foundry_status[number_of_foundries];
	foundryMutexes = (pthread_mutex_t*)malloc(number_of_foundries*sizeof(pthread_mutex_t));

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

  //Thread creation

  //Miners
  pthread_t miner_tid[number_of_mine];
  for(unsigned int i = 0; i < number_of_mine; i++) {
    miner_status[i] = 0x01;
    MinerArgs args;
    args.minerInfo = miners[i];
    args.mine_resource = mine_resource[i];
    args.miner_time = miner_times[i];
    InitWriteOutput();
    pthread_create(&miner_tid[i],NULL,main_miner_routine,(void *) &args);
  }

	//Smelter
	pthread_t smelter_tid[number_of_smelters];
	for(unsigned int i = 0; i < number_of_smelters; i++) {
		smelter_status[i] = 0x01;
		SmelterArgs args;
		args.smelterInfo = smelters[i];
		args.smelter_time = smelter_times[i];
		InitWriteOutput();
		pthread_create(&smelter_tid[i],NULL,main_smelter_routine,(void *) &args);
	}

	//Foundries
	pthread_t foundry_tid[number_of_foundries];
	for (unsigned int i = 0; i < number_of_foundries; i++) {
		foundry_status[i] = 0x01;
		FoundryArgs args;
		args.foundryInfo = foundries[i];
		args.foundry_time = foundry_times[i];
		InitWriteOutput();
		pthread_create(&foundry_tid[i],NULL,main_foundry_routine,(void *) &args);
	}

  //Transporters
  pthread_t transporter_tid[number_of_transporters];
  for(unsigned int i = 0; i < number_of_transporters; i++) {
    transporter_status[i] = 0x01;
    TransporterArgs args;
    args.transporterInfo = transporters[i];
    args.miners = miners;
    args.smelters = smelters;
    args.foundries = foundries;
    args.transport_time = transport_times[i];
    args.number_of_mine = number_of_mine;
		args.number_of_smelters = number_of_smelters;
		args.number_of_foundries = number_of_foundries;
    args.miner_status = miner_status;
    InitWriteOutput();
    pthread_create(&transporter_tid[i],NULL,main_transporter_routine,(void *) &args);
  }


  //Thread joining

  for (unsigned int i = 0; i < number_of_mine; i++) {
    pthread_join(miner_tid[i],NULL);
  }

	for (unsigned int i = 0; i < number_of_smelters; i++) {
		pthread_join(smelter_tid[i],NULL);
	}

	for (unsigned int i = 0; i < number_of_foundries; i++) {
		pthread_join(foundry_tid[i],NULL);
	}
  for (unsigned int i = 0; i < number_of_transporters; i++) {
    pthread_join(transporter_tid[i],NULL);
  }


  return 0;

}
