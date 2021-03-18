void die(const char *msg, int status, ...);

#define WHERE {                                                       \
    printf("[DBG]: %s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__);  \
  }

#define WHERED(desc) {                                                         \
    printf("[DBG]: %s:%d %s(): %s\n", __FILE__, __LINE__, __FUNCTION__, desc); \
  }
