set(header
    AnimationTest.h
    BBoxTest.h
	CacheTest.h
	ContextTest.h
    DirectoryModelTest.h
    DirectoryWatcherTest.h
    DrivesModelTest.h
    EnumTest.h
    ErrorTest.h
    EventTest.h
    FileIOTest.h
    FileInfoTest.h
	FrameTest.h
	IEventSystemTest.h
	ISystemTest.h
    ListObserverTest.h
    LogSystemTest.h
    MapObserverTest.h
    MathTest.h
    MemoryTest.h
    OSTest.h
    ObjectTest.h
    PathTest.h
	PicoJSONTest.h
	RangeTest.h
	SpeedTest.h
    StringTest.h
    TextSystemTest.h
    TimeTest.h
    ValueObserverTest.h
    VectorTest.h)
set(source
    AnimationTest.cpp
    BBoxTest.cpp
	CacheTest.cpp
	ContextTest.cpp
    DirectoryModelTest.cpp
    DirectoryWatcherTest.cpp
    DrivesModelTest.cpp
    EnumTest.cpp
    ErrorTest.cpp
    EventTest.cpp
    FileIOTest.cpp
    FileInfoTest.cpp
	FrameTest.cpp
	IEventSystemTest.cpp
	ISystemTest.cpp
    ListObserverTest.cpp
    LogSystemTest.cpp
    MapObserverTest.cpp
    MathTest.cpp
    MemoryTest.cpp
    OSTest.cpp
    ObjectTest.cpp
    PathTest.cpp
	PicoJSONTest.cpp
	RangeTest.cpp
	SpeedTest.cpp
    StringTest.cpp
    TextSystemTest.cpp
    TimeTest.cpp
    ValueObserverTest.cpp
    VectorTest.cpp)

add_library(djvCoreTest ${header} ${source})
target_link_libraries(djvCoreTest djvTestLib)
set_target_properties(
    djvCoreTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)

