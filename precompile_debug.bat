@SET INCLUDE=-I"..\3rdparty\boost_1_56_0"
@SET DEBUG_OPTIONS=-std=gnu++0x -DUNICODE -D_UNICODE -D_DEBUG -O0 -g3 -Wall -c -fmessage-length=0
@SET RELEASE_OPTIONS=-std=gnu++0x -DUNICODE -D_UNICODE -O3 -Wall -c -fmessage-length=0

@pushd %0\..
g++ %DEBUG_OPTIONS% %INCLUDE% "./include/stdwin.h" -o "./Debug/src/stdwin.gch"
g++ %DEBUG_OPTIONS% %INCLUDE% "./include/stdboost.h" -o "./Debug/src/stdboost.gch"


