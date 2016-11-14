set(SRC DataBase.cpp
        DataBase.h
        Snapshot.cpp
        Snapshot.h
        Iterator.cpp
        Iterator.h
        db_types.h
        )

add_library(db
        ${SRC})

target_link_libraries(db
        leveldb_static)