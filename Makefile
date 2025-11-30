CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread
LDFLAGS = -lcrypto -lboost_program_options -lboost_system -lboost_filesystem
SRCDIR = src
TESTDIR = tests
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = server
TEST_TARGET = test_runner

# Используем wildcard для автоматического поиска всех тестовых файлов
TEST_SOURCES = $(wildcard $(TESTDIR)/test_*.cpp)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SOURCES) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -o $(TESTDIR)/$@ $(TEST_SOURCES) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES)) $(LDFLAGS) -lUnitTest++

test: $(TEST_TARGET)
	cd $(TESTDIR) && ./$(TEST_TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(TESTDIR)/$(TEST_TARGET) test.log test_log.log test_users.conf test_auth.conf integration_users.conf

.PHONY: clean test