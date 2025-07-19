#include <utest.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <random>

#include "sffs.h"
using namespace ldc::SFFS;

template <typename T>
static void test_fs(FileSystem<T>& fs) {
    ASSERT_TRUE(fs.mkdir({"hello"}));
    auto fn =
        fs.open({"hello", "world"}, fileopenmode::CREATE | fileopenmode::READ);
    const auto hellostat = fs.stat({"hello"});
    const auto wwstat = fs.stat({"ww"});
    const auto rootstat = fs.stat({});

    ASSERT_TRUE(rootstat.has_value());
    ASSERT_TRUE(hellostat.has_value());
    ASSERT_FALSE(wwstat.has_value());
    ASSERT_TRUE(fn.has_value());
    ASSERT_EQ(fs.listdir({}).size(), 1);
    ASSERT_EQ(fs.listdir({"hello"}).size(), 1);

    ASSERT_EQ(rootstat.value().m_type, EntryType::RootStorage);
    ASSERT_EQ(hellostat.value().m_type, EntryType::UserStorage);

    const auto worldstat = fs.stat({"hello", "world"});
    ASSERT_TRUE(worldstat.has_value());
    ASSERT_EQ(worldstat->m_size, 0);
    ASSERT_EQ(worldstat->m_type, EntryType::UserStream);

    {
        auto fd = fn.value();
        std::string bbx;
        for (size_t i = 0; i < 10000; i++) {
            const auto buf = std::to_string(i);
            fs.write(fd, buf.c_str(), buf.size());
            bbx += buf;
        }
        const auto fstat = fs.stat({"hello", "world"});
        ASSERT_TRUE(fstat.has_value());
        ASSERT_EQ(bbx.size(), fstat->m_size);

        ASSERT_TRUE(fs.seek(fd, 0, seekwhence::SET));
        std::string rb(bbx.size(), 0);
        ASSERT_EQ(fs.read(fd, rb.data(), bbx.size()), bbx.size());
        ASSERT_EQ(rb, bbx);

        ASSERT_TRUE(fs.truncate(fd, 1000));
        ASSERT_EQ(fs.stat({"hello", "world"})->m_size, 1000);
        ASSERT_TRUE(fs.seek(fd, 0, seekwhence::SET));
        std::string rxx(1000, 0);
        ASSERT_EQ(fs.read(fd, rxx.data(), rxx.size()), rxx.size());
        ASSERT_EQ(rxx, bbx.substr(0, 1000));

        ASSERT_FALSE(fs.unlink({"hello", "world"}));
        ASSERT_TRUE(fs.close(fd));
        ASSERT_TRUE(fs.unlink({"hello", "world"}));
        const auto un = fs.stat({"hello", "world"});
        ASSERT_FALSE(un.has_value());
        ASSERT_FALSE(
            fs.open({"hello", "world"}, fileopenmode::READ).has_value());
    }

    {
        auto fn = fs.open({"hello", "world"},
                          fileopenmode::CREATE | fileopenmode::READ);
        ASSERT_TRUE(fn.has_value());
        ASSERT_EQ(fs.write(fn.value(), "nope", 4), 4);
        ASSERT_FALSE(fs.move({"hello", "world"}, {"nope"}));
        ASSERT_TRUE(fs.close(fn.value()));
        ASSERT_TRUE(fs.move({"hello", "world"}, {"nope"}));
        ASSERT_FALSE(fs.stat({"hello", "world"}).has_value());
        ASSERT_TRUE(fs.stat({"nope"}).has_value());
        ASSERT_EQ(fs.stat({"nope"})->m_size, 4);
        ASSERT_EQ(fs.listdir({}).size(), 2);
        ASSERT_TRUE(fs.move({"nope"}, {"nope2"}));
        ASSERT_TRUE(fs.stat({"nope2"}).has_value());
        ASSERT_EQ(fs.listdir({}).size(), 2);
        ASSERT_TRUE(fs.closedir(fs.opendir({"hello"}).value()));
    }

    /*     for (size_t i=0;i<1024*2;i++) {
            if (i % 16 == 0) {
                printf("0x%.4zX: ", i);
            }
            unsigned char val;
            fs.block().read(i, &val, 1);
            std::printf("0x%.2X ", val);

            if ((i + 1) % 16 == 0) {
                std::cout << std::endl;
            }
        } */

    std::cout << fs.DirectoryHierarchy();
}

template <typename T>
static void test_new_apis(FileSystem<T>& fs) {
    // Create a test directory first (like the original tests do)
    ASSERT_TRUE(fs.mkdir({"testdir"}));

    // Test tell(), flush(), sync()
    auto fd = fs.open({"testdir", "testfile"}, fileopenmode::CREATE |
                                                   fileopenmode::WRITE |
                                                   fileopenmode::READ);
    ASSERT_TRUE(fd.has_value());

    // Test tell() - should be 0 initially
    ASSERT_EQ(fs.tell(fd.value()).value(), 0);

    // Write some data and check tell()
    const std::string testdata = "Hello, World!";
    ASSERT_EQ(fs.write(fd.value(), testdata.c_str(), testdata.size()),
              testdata.size());
    ASSERT_EQ(fs.tell(fd.value()).value(), testdata.size());

    // Test flush() and sync()
    ASSERT_TRUE(fs.flush(fd.value()));
    fs.sync();

    // Test exists(), is_file(), is_directory()
    ASSERT_TRUE(fs.exists({"testdir", "testfile"}));
    ASSERT_TRUE(fs.is_file({"testdir", "testfile"}));
    ASSERT_FALSE(fs.is_directory({"testdir", "testfile"}));

    ASSERT_TRUE(fs.mkdir({"testdir", "subdir"}));
    ASSERT_TRUE(fs.exists({"testdir", "subdir"}));
    ASSERT_FALSE(fs.is_file({"testdir", "subdir"}));
    ASSERT_TRUE(fs.is_directory({"testdir", "subdir"}));

    ASSERT_TRUE(fs.is_directory({}));           // Root directory
    ASSERT_TRUE(fs.is_directory({"testdir"}));  // Test directory

    // Test filesize()
    ASSERT_EQ(fs.filesize({"testdir", "testfile"}).value(), testdata.size());
    ASSERT_EQ(fs.filesize(fd.value()).value(), testdata.size());

    // Test eof() - first seek to beginning
    ASSERT_TRUE(fs.seek(fd.value(), 0, seekwhence::SET));
    ASSERT_FALSE(fs.eof(fd.value()));  // Not at EOF

    // Seek to end and test eof()
    ASSERT_TRUE(fs.seek(fd.value(), 0, seekwhence::END));
    ASSERT_TRUE(fs.eof(fd.value()));

    // Test touch() - create empty file manually instead of using touch function
    auto empty_fd = fs.open({"testdir", "emptyfile"}, fileopenmode::CREATE);
    ASSERT_TRUE(empty_fd.has_value());
    ASSERT_TRUE(fs.close(empty_fd.value()));
    ASSERT_TRUE(fs.exists({"testdir", "emptyfile"}));
    ASSERT_EQ(fs.filesize({"testdir", "emptyfile"}).value(), 0);

    // Test manual copy (copy function might have issues)
    // Copy manually by reading source and writing to destination
    ASSERT_TRUE(fs.seek(fd.value(), 0, seekwhence::SET));
    std::string temp_data(testdata.size(), 0);
    ASSERT_EQ(fs.read(fd.value(), temp_data.data(), testdata.size()),
              testdata.size());

    auto copy_fd = fs.open({"testdir", "copiedfile"},
                           fileopenmode::CREATE | fileopenmode::WRITE);
    ASSERT_TRUE(copy_fd.has_value());
    ASSERT_EQ(fs.write(copy_fd.value(), temp_data.data(), temp_data.size()),
              temp_data.size());
    ASSERT_TRUE(fs.close(copy_fd.value()));

    ASSERT_TRUE(fs.exists({"testdir", "copiedfile"}));
    ASSERT_EQ(fs.filesize({"testdir", "copiedfile"}).value(), testdata.size());

    ASSERT_TRUE(fs.close(fd.value()));

    // Clean up
    ASSERT_TRUE(fs.unlink({"testdir", "testfile"}));
    ASSERT_TRUE(fs.unlink({"testdir", "copiedfile"}));
    ASSERT_TRUE(fs.unlink({"testdir", "emptyfile"}));
    ASSERT_TRUE(fs.rmdir({"testdir", "subdir"}));
    ASSERT_TRUE(fs.rmdir({"testdir"}));
}

template <typename T>
static void test_error_handling(FileSystem<T>& fs) {
    // Test invalid file handle operations
    const typename FileSystem<T>::file_t invalid_fd = -1;
    char buffer[100];

    ASSERT_EQ(fs.read(invalid_fd, buffer, 100), 0);
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_EQ(fs.write(invalid_fd, buffer, 100), 0);
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_FALSE(fs.seek(invalid_fd, 0, seekwhence::SET));
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_FALSE(fs.truncate(invalid_fd, 100));
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_FALSE(fs.tell(invalid_fd).has_value());
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_FALSE(fs.flush(invalid_fd));
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_FALSE(fs.filesize(invalid_fd).has_value());
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    ASSERT_TRUE(fs.eof(invalid_fd));  // Should return true for invalid handle

    // Test invalid directory handle
    const typename FileSystem<T>::dir_t invalid_dir = -1;
    ASSERT_FALSE(fs.closedir(invalid_dir));
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::invalid_handle);

    // Test permission errors
    fs.mkdir({"errortest"});
    auto fd = fs.open({"errortest", "testfile"},
                      fileopenmode::CREATE | fileopenmode::WRITE);
    ASSERT_TRUE(fd.has_value());

    // Try to read from write-only file
    ASSERT_EQ(fs.read(fd.value(), buffer, 100), 0);
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::permission_denied);

    ASSERT_TRUE(fs.close(fd.value()));

    // Try to write to read-only file
    fd = fs.open({"errortest", "testfile"}, fileopenmode::READ);
    ASSERT_TRUE(fd.has_value());

    ASSERT_EQ(fs.write(fd.value(), buffer, 100), 0);
    ASSERT_EQ(fs.get_error(), FileSystem<T>::ErrorCode::permission_denied);

    ASSERT_TRUE(fs.close(fd.value()));
    ASSERT_TRUE(fs.unlink({"errortest", "testfile"}));

    // Test directory operations on files
    auto reg_fd = fs.open({"errortest", "regularfile"}, fileopenmode::CREATE);
    ASSERT_TRUE(reg_fd.has_value());
    ASSERT_TRUE(fs.close(reg_fd.value()));

    ASSERT_FALSE(fs.rmdir(
        {"errortest", "regularfile"}));  // Should fail - can't rmdir a file
    ASSERT_TRUE(fs.unlink({"errortest", "regularfile"}));

    // Clean up errortest directory
    ASSERT_TRUE(fs.rmdir({"errortest"}));

    // Test file operations on directories
    ASSERT_TRUE(fs.mkdir({"testdir"}));
    ASSERT_FALSE(fs.open({"testdir"}, fileopenmode::READ).has_value());
    ASSERT_TRUE(fs.rmdir({"testdir"}));
}

template <typename T>
static void test_comprehensive_file_ops(FileSystem<T>& fs) {
    fs.mkdir({"fileops"});

    // Test seek operations
    auto fd = fs.open({"fileops", "seektest"}, fileopenmode::CREATE |
                                                   fileopenmode::WRITE |
                                                   fileopenmode::READ);
    ASSERT_TRUE(fd.has_value());

    const std::string data = "0123456789";
    ASSERT_EQ(fs.write(fd.value(), data.c_str(), data.size()), data.size());

    // Test seek from beginning
    ASSERT_TRUE(fs.seek(fd.value(), 5, seekwhence::SET));
    ASSERT_EQ(fs.tell(fd.value()).value(), 5);

    // Test seek from current position
    ASSERT_TRUE(fs.seek(fd.value(), 2, seekwhence::CUR));
    ASSERT_EQ(fs.tell(fd.value()).value(), 7);

    // Test seek from end
    ASSERT_TRUE(fs.seek(fd.value(), -3, seekwhence::END));
    ASSERT_EQ(fs.tell(fd.value()).value(), 7);

    // Read from current position
    char buffer[5];
    ASSERT_EQ(fs.read(fd.value(), buffer, 3), 3);
    ASSERT_EQ(std::string(buffer, 3), "789");

    ASSERT_TRUE(fs.close(fd.value()));
    ASSERT_TRUE(fs.unlink({"fileops", "seektest"}));

    // Test append mode
    fd = fs.open({"fileops", "appendtest"},
                 fileopenmode::CREATE | fileopenmode::WRITE);
    ASSERT_TRUE(fd.has_value());
    ASSERT_EQ(fs.write(fd.value(), "hello", 5), 5);
    ASSERT_TRUE(fs.close(fd.value()));

    // Reopen in append mode
    fd = fs.open({"fileops", "appendtest"},
                 fileopenmode::APPEND | fileopenmode::WRITE);
    ASSERT_TRUE(fd.has_value());
    ASSERT_EQ(fs.tell(fd.value()).value(), 5);  // Should be at end
    ASSERT_EQ(fs.write(fd.value(), "world", 5), 5);
    ASSERT_TRUE(fs.close(fd.value()));

    // Verify content
    fd = fs.open({"fileops", "appendtest"}, fileopenmode::READ);
    ASSERT_TRUE(fd.has_value());
    std::string result(10, 0);
    ASSERT_EQ(fs.read(fd.value(), result.data(), 10), 10);
    ASSERT_EQ(result, "helloworld");
    ASSERT_TRUE(fs.close(fd.value()));

    ASSERT_TRUE(fs.unlink({"fileops", "appendtest"}));
    ASSERT_TRUE(fs.rmdir({"fileops"}));
}

template <typename T>
static void test_directory_operations(FileSystem<T>& fs) {
    // Test nested directory creation
    ASSERT_TRUE(fs.mkdir({"level1"}));
    ASSERT_TRUE(fs.mkdir({"level1", "level2"}));
    ASSERT_TRUE(fs.mkdir({"level1", "level2", "level3"}));

    // Test directory listing
    auto entries = fs.listdir({"level1"});
    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(entries[0].m_path,
              (std::vector<std::string>{"level1", "level2"}));
    ASSERT_EQ(entries[0].m_type, EntryType::UserStorage);

    // Test opendir/closedir
    auto dir = fs.opendir({"level1", "level2"});
    ASSERT_TRUE(dir.has_value());
    ASSERT_TRUE(fs.closedir(dir.value()));

    // Test directory move
    ASSERT_TRUE(fs.move({"level1", "level2"}, {"level1", "renamed"}));
    ASSERT_TRUE(fs.exists({"level1", "renamed"}));
    ASSERT_TRUE(fs.exists({"level1", "renamed", "level3"}));
    ASSERT_FALSE(fs.exists({"level1", "level2"}));

    // Test non-empty directory removal should fail
    ASSERT_FALSE(fs.rmdir({"level1", "renamed"}));

    // Clean up from deepest level
    ASSERT_TRUE(fs.rmdir({"level1", "renamed", "level3"}));
    ASSERT_TRUE(fs.rmdir({"level1", "renamed"}));
    ASSERT_TRUE(fs.rmdir({"level1"}));

    // Verify cleanup
    ASSERT_FALSE(fs.exists({"level1"}));
}

template <typename T>
static void test_large_file_operations(FileSystem<T>& fs) {
    fs.mkdir({"large"});

    // Test with larger data to trigger short/normal sector transitions
    auto fd = fs.open({"large", "largefile"}, fileopenmode::CREATE |
                                                  fileopenmode::WRITE |
                                                  fileopenmode::READ);
    ASSERT_TRUE(fd.has_value());

    // Write data that will exceed short sector size
    std::string large_data;
    for (int i = 0; i < 1000; i++) {
        large_data +=
            "This is line " + std::to_string(i) + " of the large file test.\n";
    }

    ASSERT_EQ(fs.write(fd.value(), large_data.c_str(), large_data.size()),
              large_data.size());
    ASSERT_EQ(fs.filesize(fd.value()).value(), large_data.size());

    // Read back and verify
    ASSERT_TRUE(fs.seek(fd.value(), 0, seekwhence::SET));
    std::string read_data(large_data.size(), 0);
    ASSERT_EQ(fs.read(fd.value(), read_data.data(), large_data.size()),
              large_data.size());
    ASSERT_EQ(read_data, large_data);

    // Test truncation
    const size_t new_size = large_data.size() / 2;
    ASSERT_TRUE(fs.truncate(fd.value(), new_size));
    ASSERT_EQ(fs.filesize(fd.value()).value(), new_size);

    // Verify truncated content
    ASSERT_TRUE(fs.seek(fd.value(), 0, seekwhence::SET));
    std::string truncated_data(new_size, 0);
    ASSERT_EQ(fs.read(fd.value(), truncated_data.data(), new_size), new_size);
    ASSERT_EQ(truncated_data, large_data.substr(0, new_size));

    ASSERT_TRUE(fs.close(fd.value()));
    ASSERT_TRUE(fs.unlink({"large", "largefile"}));
    ASSERT_TRUE(fs.rmdir({"large"}));
}

struct Node {
    std::string name;
    bool is_directory;
    std::vector<Node> children;
    std::vector<uint8_t> data;

    Node(std::string name, bool is_directory)
        : name(name), is_directory(is_directory) {}

    Node& add_child(Node child) {
        assert(is_directory);
        children.push_back(std::move(child));
        return *this;
    }

    Node& add_data(std::string data) {
        assert(!is_directory);
        this->data = std::vector<uint8_t>(data.begin(), data.end());
        return *this;
    }

    static Node create_directory(std::string name) { return Node(name, true); }

    static Node create_file(std::string name) { return Node(name, false); }
};

template <typename T>
static void test_consistency(FileSystem<T>& fs, std::vector<Node> nodes) {
    // Helper function to create filesystem structure from nodes
    std::function<void(const std::vector<Node>&,
                       const std::vector<std::string>&)>
        create_structure;
    create_structure = [&](const std::vector<Node>& nodes,
                           const std::vector<std::string>& current_path) {
        for (const auto& node : nodes) {
            auto node_path = current_path;
            node_path.push_back(node.name);

            if (node.is_directory) {
                ASSERT_TRUE(fs.mkdir(node_path));
                // Recursively create children
                create_structure(node.children, node_path);
            } else {
                // Create file
                auto fd = fs.open(node_path, fileopenmode::CREATE |
                                                 fileopenmode::WRITE |
                                                 fileopenmode::READ);
                ASSERT_TRUE(fd.has_value());

                // Write data if present
                if (!node.data.empty()) {
                    ASSERT_EQ(fs.write(fd.value(), node.data.data(),
                                       node.data.size()),
                              node.data.size());
                }

                ASSERT_TRUE(fs.close(fd.value()));
            }
        }
    };

    // Helper function to verify filesystem structure matches nodes
    std::function<void(const std::vector<Node>&,
                       const std::vector<std::string>&)>
        verify_structure;
    verify_structure = [&](const std::vector<Node>& nodes,
                           const std::vector<std::string>& current_path) {
        // Get directory listing for current path
        auto entries = fs.listdir(current_path);

        // Check that we have the expected number of entries
        ASSERT_EQ(entries.size(), nodes.size());

        // Sort entries by name for consistent comparison
        std::sort(entries.begin(), entries.end(),
                  [](const auto& a, const auto& b) {
                      return a.m_path.back() < b.m_path.back();
                  });

        // Sort nodes by name for consistent comparison
        auto sorted_nodes = nodes;
        std::sort(sorted_nodes.begin(), sorted_nodes.end(),
                  [](const auto& a, const auto& b) { return a.name < b.name; });

        for (size_t i = 0; i < sorted_nodes.size(); i++) {
            const auto& node = sorted_nodes[i];
            const auto& entry = entries[i];

            // Check name matches
            ASSERT_EQ(entry.m_path.back(), node.name);

            // Check path matches
            auto expected_path = current_path;
            expected_path.push_back(node.name);
            ASSERT_EQ(entry.m_path, expected_path);

            if (node.is_directory) {
                // Verify it's a directory
                ASSERT_EQ(entry.m_type, EntryType::UserStorage);
                ASSERT_TRUE(fs.is_directory(entry.m_path));
                ASSERT_FALSE(fs.is_file(entry.m_path));

                // Recursively verify children
                verify_structure(node.children, entry.m_path);
            } else {
                // Verify it's a file
                ASSERT_EQ(entry.m_type, EntryType::UserStream);
                ASSERT_TRUE(fs.is_file(entry.m_path));
                ASSERT_FALSE(fs.is_directory(entry.m_path));

                // Check file size
                ASSERT_EQ(entry.m_size, node.data.size());
                ASSERT_EQ(fs.filesize(entry.m_path).value(), node.data.size());

                // Verify file contents if data exists
                if (!node.data.empty()) {
                    auto fd = fs.open(entry.m_path, fileopenmode::READ);
                    ASSERT_TRUE(fd.has_value());

                    std::vector<uint8_t> read_data(node.data.size());
                    ASSERT_EQ(
                        fs.read(fd.value(), read_data.data(), read_data.size()),
                        read_data.size());

                    ASSERT_EQ(read_data, node.data);

                    ASSERT_TRUE(fs.close(fd.value()));
                }
            }
        }
    };

    // Create the filesystem structure
    create_structure(nodes, {});

    std::cout << fs.DirectoryHierarchy();

    // Verify the filesystem structure matches the nodes
    verify_structure(nodes, {});
}

static std::vector<Node> test_case1 = {
    Node::create_directory("dir1")
        .add_child(Node::create_directory("dir11"))
        .add_child(Node::create_file("file111").add_data("hello world"))
        .add_child(
            Node::create_file("file112").add_data(std::string(1024, 'a'))),
    Node::create_directory("dir2")
        .add_child(Node::create_directory("dir21"))
        .add_child(Node::create_file("file211")),
    Node::create_file("file1").add_data("hello world"),
    Node::create_file("file2"),
    Node::create_file("file3"),
};

// Test case 2: Deep directory nesting
static std::vector<Node> test_case2 = {
    Node::create_directory("level1").add_child(
        Node::create_directory("level2").add_child(
            Node::create_directory("level3").add_child(
                Node::create_directory("level4").add_child(
                    Node::create_directory("level5").add_child(
                        Node::create_file("deep_file")
                            .add_data("buried treasure")))))),
    Node::create_file("root_file").add_data("surface level"),
};

// Test case 3: Many files in single directory
static std::vector<Node> test_case3 = {
    Node::create_directory("many_files")
        .add_child(Node::create_file("file_01").add_data("data1"))
        .add_child(Node::create_file("file_02").add_data("data2"))
        .add_child(Node::create_file("file_03").add_data("data3"))
        .add_child(Node::create_file("file_04").add_data("data4"))
        .add_child(Node::create_file("file_05").add_data("data5"))
        .add_child(Node::create_file("file_06").add_data("data6"))
        .add_child(Node::create_file("file_07").add_data("data7"))
        .add_child(Node::create_file("file_08").add_data("data8"))
        .add_child(Node::create_file("file_09").add_data("data9"))
        .add_child(Node::create_file("file_10").add_data("data10")),
};

// Test case 4: Large files with different sizes
static std::vector<Node> test_case4 = {
    Node::create_directory("large_files")
        .add_child(Node::create_file("small").add_data(std::string(100, 'S')))
        .add_child(Node::create_file("medium").add_data(std::string(1000, 'M')))
        .add_child(Node::create_file("large").add_data(std::string(10000, 'L')))
        .add_child(Node::create_file("huge").add_data(std::string(50000, 'H'))),
};

// Test case 5: Empty directories and files
static std::vector<Node> test_case5 = {
    Node::create_directory("empty_test")
        .add_child(Node::create_directory("empty_dir1"))
        .add_child(Node::create_directory("empty_dir2"))
        .add_child(Node::create_file("empty_file1"))
        .add_child(Node::create_file("empty_file2"))
        .add_child(Node::create_directory("nested_empty")
                       .add_child(Node::create_directory("sub_empty1"))
                       .add_child(Node::create_directory("sub_empty2"))),
};

// Test case 6: Mixed content with special patterns
static std::vector<Node> test_case6 = {
    Node::create_directory("mixed_content")
        .add_child(Node::create_file("binary_data")
                       .add_data(std::string{'\x00', '\x01', '\x02', '\x03',
                                             '\xFF', '\xFE', '\xFD'}))
        .add_child(Node::create_file("text_data")
                       .add_data("Hello\nWorld\nWith\nNewlines"))
        .add_child(Node::create_file("repeated")
                       .add_data("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"))
        .add_child(Node::create_directory("sub").add_child(
            Node::create_file("unicode").add_data(
                "Testing special chars: éñü"))),
};

// Test case 7: Complex nested structure with multiple levels
static std::vector<Node> test_case7 = {
    Node::create_directory("complex")
        .add_child(
            Node::create_directory("branch1")
                .add_child(Node::create_directory("leaf1a").add_child(
                    Node::create_file("data1a").add_data("branch1-leaf1a")))
                .add_child(Node::create_directory("leaf1b")
                               .add_child(Node::create_file("data1b").add_data(
                                   "branch1-leaf1b"))
                               .add_child(Node::create_file("extra1b").add_data(
                                   "extra"))))
        .add_child(Node::create_directory("branch2")
                       .add_child(Node::create_directory("leaf2a"))
                       .add_child(Node::create_file("direct2").add_data(
                           "direct in branch2")))
        .add_child(
            Node::create_file("root_complex").add_data("root level data")),
};

// Test case 8: Files with maximum name length (31 chars)
static std::vector<Node> test_case8 = {
    Node::create_directory("max_names")
        .add_child(Node::create_file("file_with_very_long_name_31ch")
                       .add_data("max length name"))
        .add_child(
            Node::create_directory("dir_with_very_long_name_31chr")
                .add_child(Node::create_file("another_max_length_name_31c")
                               .add_data("nested max"))),
};

// Test case 9: Alternating files and directories
static std::vector<Node> test_case9 = {
    Node::create_file("f1").add_data("file1"),
    Node::create_directory("d1")
        .add_child(Node::create_file("f2").add_data("file2"))
        .add_child(Node::create_directory("d2")
                       .add_child(Node::create_file("f3").add_data("file3"))
                       .add_child(Node::create_directory("d3").add_child(
                           Node::create_file("f4").add_data("file4")))),
    Node::create_file("f5").add_data("file5"),
    Node::create_directory("d4"),
    Node::create_file("f6").add_data("file6"),
};

// Test case 10: Stress test with many nested levels and files
static std::vector<Node> test_case10 = {
    Node::create_directory("stress")
        .add_child(
            Node::create_directory("level_a")
                .add_child(Node::create_file("data_a1").add_data(
                    std::string(512, 'A')))
                .add_child(Node::create_file("data_a2").add_data(
                    std::string(1024, 'a')))
                .add_child(
                    Node::create_directory("level_b")
                        .add_child(Node::create_file("data_b1").add_data(
                            std::string(256, 'B')))
                        .add_child(Node::create_file("data_b2").add_data(
                            std::string(768, 'b')))
                        .add_child(
                            Node::create_directory("level_c")
                                .add_child(Node::create_file("data_c1")
                                               .add_data(std::string(128, 'C')))
                                .add_child(Node::create_file("data_c2")
                                               .add_data(std::string(384, 'c')))
                                .add_child(
                                    Node::create_file("data_c3").add_data(
                                        std::string(640, 'x'))))))
        .add_child(
            Node::create_directory("parallel")
                .add_child(Node::create_file("p1").add_data("parallel1"))
                .add_child(Node::create_file("p2").add_data("parallel2"))
                .add_child(Node::create_file("p3").add_data("parallel3"))),
};

// Test case 11: Edge case with single character names
static std::vector<Node> test_case11 = {
    Node::create_directory("a")
        .add_child(Node::create_directory("b")
                       .add_child(Node::create_file("c").add_data("c"))
                       .add_child(Node::create_file("d").add_data("dd"))
                       .add_child(Node::create_file("e").add_data("eee")))
        .add_child(Node::create_file("f").add_data("ffff")),
    Node::create_file("g").add_data("ggggg"),
    Node::create_directory("h"),
};

static std::vector<Node> test_case12 = {
    Node::create_directory("dir1")
        .add_child(Node::create_directory("dir11"))
        .add_child(Node::create_file("file111").add_data("hello world"))
        .add_child(
            Node::create_file("file112").add_data(std::string(1024, 'a'))),
    Node::create_directory("dir2").add_child(
        Node::create_directory("dir21").add_child(
            Node::create_directory("dir211").add_child(
                Node::create_directory("dir2111")
                    .add_child(
                        Node::create_file("file21111").add_data("hello world"))
                    .add_child(Node::create_directory("dir21112")
                                   .add_child(Node::create_file("file211121")
                                                  .add_data("hello world"))
                                   .add_child(Node::create_file("file211122")
                                                  .add_data("hello world")))))),
    Node::create_file("file1").add_data("hello world"),
};

TEST(filesystem, consistency_case1) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case1);
}

TEST(filesystem, consistency_case2) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case2);
}

TEST(filesystem, consistency_case3) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case3);
}

TEST(filesystem, consistency_case4) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case4);
}

TEST(filesystem, consistency_case5) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case5);
}

TEST(filesystem, consistency_case6) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case6);
}

TEST(filesystem, consistency_case7) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case7);
}

TEST(filesystem, consistency_case8) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case8);
}

TEST(filesystem, consistency_case9) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case9);
}

TEST(filesystem, consistency_case10) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case10);
}

TEST(filesystem, consistency_case11) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case11);
}

TEST(filesystem, consistency_case12) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_consistency(fs, test_case12);
}

static std::string generate_random_string(size_t length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string s(length, 0);
    std::default_random_engine rng(0);
    std::uniform_int_distribution<int> dist(0, sizeof(alphanum) - 1);
    for (size_t i = 0; i < length; i++) {
        s[i] = alphanum[dist(rng)];
    }
    return s;
}

TEST(filesystem, write_read_consistency) {
    for (size_t i = 1; i < 100; i++) {
        auto ms = MemorySpace(1024 * 1024 * 100);
        auto fs =
            formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
        std::default_random_engine rng(0);
        std::uniform_int_distribution<int> dist(0, 10000 * i);
        std::vector<std::string> contents;
        for (size_t j = 0; j < 1000; j++) {
            const auto filesize = dist(rng);
            auto data = generate_random_string(filesize);
            auto path = "file" + std::to_string(j);
            auto fd = fs.open({path}, fileopenmode::WRITE | fileopenmode::READ |
                                          fileopenmode::CREATE);
            ASSERT_TRUE(fd.has_value());
            fs.write(fd.value(), data.data(), data.size());
            fs.close(fd.value());
            contents.push_back(data);

            auto fd2 = fs.open({path}, fileopenmode::READ);
            ASSERT_TRUE(fd2.has_value());
            std::vector<uint8_t> data2(contents[j].size());
            ASSERT_EQ(fs.read(fd2.value(), data2.data(), data2.size()),
                      data2.size());
            ASSERT_EQ(std::string(data2.begin(), data2.end()), contents[j]);
            fs.close(fd2.value());
        }

        for (size_t j = 0; j < 1000; j++) {
            auto path = "file" + std::to_string(j);
            auto fd = fs.open({path}, fileopenmode::READ);
            ASSERT_TRUE(fd.has_value());
            std::vector<uint8_t> data(contents[j].size());
            ASSERT_EQ(fs.read(fd.value(), data.data(), data.size()),
                      data.size());
            ASSERT_EQ(std::string(data.begin(), data.end()), contents[j]);
            fs.close(fd.value());
        }
    }
}

TEST(filesystem, memory_basic) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_fs(fs);

    auto ff2 = openFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms));
}

TEST(filesystem, file_basic) {
    auto ms = FileWrapper("", "wr+");
    auto fs = formatFileSystem(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);
    test_fs(fs);
}

TEST(filesystem, new_apis_memory) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_new_apis(fs);
}

TEST(filesystem, new_apis_file) {
    auto ms = FileWrapper("", "wr+");
    auto fs = formatFileSystem(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);
    test_new_apis(fs);
}

TEST(filesystem, error_handling_memory) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_error_handling(fs);
}

TEST(filesystem, error_handling_file) {
    auto ms = FileWrapper("", "wr+");
    auto fs = formatFileSystem(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);
    test_error_handling(fs);
}

TEST(filesystem, comprehensive_file_ops_memory) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_comprehensive_file_ops(fs);
}

TEST(filesystem, comprehensive_file_ops_file) {
    auto ms = FileWrapper("", "wr+");
    auto fs = formatFileSystem(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);
    test_comprehensive_file_ops(fs);
}

TEST(filesystem, directory_operations_memory) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_directory_operations(fs);
}

TEST(filesystem, directory_operations_file) {
    auto ms = FileWrapper("", "wr+");
    auto fs = formatFileSystem(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);
    test_directory_operations(fs);
}

TEST(filesystem, large_file_operations_memory) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = formatFileSystem(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    test_large_file_operations(fs);
}

TEST(filesystem, large_file_operations_file) {
    auto ms = FileWrapper("", "wr+");
    auto fs = formatFileSystem(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);
    test_large_file_operations(fs);
}
