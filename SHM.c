struct SHM {
   int shmid;
   key_t key;
   char *shm, *s;
};

struct SHM *SHM_new () {
  struct *SHM shm;
  shm = malloc(sizeof(struct SHM))
  return shm;
}

SHM_malloc(size_t n){

    key = 5678; // getkey

    if ((shmid = shmget(key, n, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return(-1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return(-2);
    }
}

int SHM_get(int key){

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    // shm lock
    return shm;

}
