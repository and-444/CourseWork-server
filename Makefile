CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread
LDFLAGS = -lcrypto -lboost_program_options -lboost_system -lboost_filesystem
SRCDIR = src
TESTDIR = tests
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = server
TEST_TARGET = test_runner

# тестовые файлы
TEST_SOURCES = \
    $(TESTDIR)/test_authmanager.cpp \
    $(TESTDIR)/test_vectorprocessor.cpp \
    $(TESTDIR)/test_sha256.cpp \
    $(TESTDIR)/test_logger.cpp \
    $(TESTDIR)/test_integration.cpp \
    $(TESTDIR)/test_interface.cpp \
    $(TESTDIR)/test_main.cpp

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SOURCES) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -o $(TESTDIR)/$@ $(TEST_SOURCES) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES)) $(LDFLAGS) -lUnitTest++

test: $(TEST_TARGET)
	cd $(TESTDIR) && ./$(TEST_TARGET)

functional-test:
	chmod +x tests/run_functional_tests.sh
	./tests/run_functional_tests.sh

doxygen:
	doxygen Doxyfile
	cd docs/latex && make
	cp docs/latex/refman.pdf NetworkServer_Documentation.pdf

pdf: doxygen

clean:
	rm -f $(OBJECTS) $(TARGET) $(TESTDIR)/$(TEST_TARGET) test.log test_log.log test_users.conf test_auth.conf integration_users.conf acceptance_users.conf

.PHONY: clean test functional-test doxygen pdf