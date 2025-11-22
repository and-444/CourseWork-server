CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread
LDFLAGS = -lcrypto -lUnitTest++
SRCDIR = src
TESTDIR = tests
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = server
TEST_TARGET = test_runner
TEST_SOURCES = $(TESTDIR)/test_authmanager.cpp $(TESTDIR)/test_vectorprocessor.cpp $(TESTDIR)/test_sha256.cpp $(TESTDIR)/test_logger.cpp $(TESTDIR)/test_integration.cpp $(TESTDIR)/test_main.cpp

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SOURCES) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -o $(TESTDIR)/$@ $(TEST_SOURCES) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES)) $(LDFLAGS)

test: $(TEST_TARGET)
	cd $(TESTDIR) && ./$(TEST_TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(TESTDIR)/$(TEST_TARGET) test.log test_log.log test_users.conf test_auth.conf integration_users.conf

.PHONY: clean test