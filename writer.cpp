#include<iostream>
#include<fcntl.h>
#include<sys/mman.h>
#include<cstring>
#include<unistd.h>



const char* SHM_NAME = "/my_shm";
const size_t SHM_SIZE = 1024;

int main(){
    int shm_fd=shm_open(SHM_NAME,O_CREAT | O_RDWR ,0666);
    if(shm_fd==-1){
        perror("shm_open");
        return 1;

    }

    ftruncate(shm_fd,SHM_SIZE);

    void* ptr=mmap(0,SHM_SIZE,PROT_WRITE,MAP_SHARED,shm_fd,0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        return 1;

    }

    const char*message="Hello Suyash1234";
    memcpy(ptr,message,strlen(message) + 1);

    std::cout<<"Sender: Message written to shared memory "<<message<<std::endl;

    munmap(ptr,SHM_SIZE);
    close(shm_fd);
    return 0;

}
