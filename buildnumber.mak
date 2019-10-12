# Create an auto-incrementing build number.

BUILD_NUMBER_FILE=build-number.txt

BUILD_NUMBER_LDFLAGS  = -Xlinker --defsym -Xlinker __BUILD_DATE=$$(date +'%Y%m%d')
BUILD_NUMBER_LDFLAGS += -Xlinker --defsym -Xlinker __BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))

# Build number file.  Increment if any object file changes.
$(BUILD_NUMBER_FILE): $(OBJECTS)
