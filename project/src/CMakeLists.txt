set(SOURCE_FILES main.c store.h store.c config.h config.c client.c network.h network.c)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(distributed-db ${SOURCE_FILES})
target_link_libraries(distributed-db Threads::Threads)

