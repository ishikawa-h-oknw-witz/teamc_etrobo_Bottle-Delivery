mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS += \
    operation/PIDCalculator.o \
    operation/DistanceCalculator.o \
    operation/TrapezoidCalculator.o \
    control/LineTraceRunner.o \
    control/GyroTraceRunner.o \
    control/ArmController.o \
    scene/SceneManager.o \
    detect/TargetDistanceDetector.o \
    Logger.o \

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += \
    $(mkfile_path)app \
    $(mkfile_path)unit \
    $(mkfile_path)control \
    $(mkfile_path)detect \
    $(mkfile_path)device \
    $(mkfile_path)operation \
    $(mkfile_path)scene \

INCLUDES += \
	-I$(mkfile_path)../../../drivers/include \
	-I$(mkfile_path)../../../drivers/include/spike \
	-I$(mkfile_path)../../../drivers/include/libcpp/spike \
	-I$(mkfile_path)control \
    -I$(mkfile_path)detect \
	-I$(mkfile_path)device \
    -I$(mkfile_path)operation \
    -I$(mkfile_path)scene \
