NDK_PATH=/home/okay/tonka/apps/android/android-ndk-r22b/
BUILD_SCRIPT=${NDK_PATH}/ndk-build

NDK_PROJECT_PATH=${PWD} ${BUILD_SCRIPT} NDK_APPLICATION_MK=./Application.mk APP_BUILD_SCRIPT=./Android.mk 
