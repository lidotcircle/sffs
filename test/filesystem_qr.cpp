#include <sffs.h>
#include <utest.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>

using namespace ldc::SFFS;

class QRFilesystemTest {
public:
    // Data structures to mirror filesystem state
    struct FileNode {
        std::string name;
        std::string content;
        size_t size;
        bool is_directory;
        std::map<std::string, std::unique_ptr<FileNode>> children;
        FileNode* parent;

        explicit FileNode(const std::string& n, bool is_dir = false,
                          FileNode* p = nullptr)
            : name(n), size(0), is_directory(is_dir), parent(p) {}
    };

    struct OpenFileInfo {
        std::string path;
        size_t position;
        fileopenmode mode;
        bool is_open;
    };

    struct OpenDirInfo {
        std::string path;
        bool is_open;
    };

    // Test state tracking
    std::unique_ptr<FileNode> root_node;
    std::map<std::string, std::string> file_contents;  // path -> content
    std::map<std::string, size_t> file_sizes;          // path -> size
    std::set<std::string> existing_paths;              // all existing paths
    std::set<std::string> directory_paths;             // directory paths only
    std::set<std::string> file_paths;                  // file paths only
    std::map<int, OpenFileInfo> open_files;            // fd -> file info
    std::map<int, OpenDirInfo> open_dirs;              // dir_fd -> dir info

    // Statistics
    size_t total_directories_created = 0;
    size_t total_files_created = 0;
    size_t total_bytes_written = 0;
    size_t total_operations = 0;

    // Test parameters - reduced to avoid exhausting filesystem capacity
    static constexpr size_t MIN_DIRECTORIES = 20;
    static constexpr size_t MIN_FILES = 40;
    static constexpr size_t MAX_DIRECTORY_DEPTH = 6;
    static constexpr size_t MAX_FILE_SIZE = 4 * 1024;  // 4KB
    static constexpr size_t MIN_FILE_SIZE = 1;

    std::mt19937 rng;
    std::uniform_int_distribution<size_t> size_dist;
    std::uniform_int_distribution<int> depth_dist;
    std::uniform_int_distribution<int> operation_dist;

    std::unique_ptr<FileSystem<MemorySpace>> fs;

    void SetUp() {
        // Initialize filesystem with 100MB memory space
        fs.reset(new FileSystem<MemorySpace>(
            formatFileSystem(MemorySpace(100 * 1024 * 1024), 3, 12, 9)));

        // Initialize root node
        root_node = std::make_unique<FileNode>("", true);
        existing_paths.insert("");
        directory_paths.insert("");

        static size_t setupCount = 0;
        rng.seed(setupCount++);
        size_dist =
            std::uniform_int_distribution<size_t>(MIN_FILE_SIZE, MAX_FILE_SIZE);
        depth_dist = std::uniform_int_distribution<int>(1, MAX_DIRECTORY_DEPTH);
        operation_dist = std::uniform_int_distribution<int>(0, 100);
    }

    void TearDown() {
        // Close all open files and directories
        for (auto& [fd, info] : open_files) {
            if (info.is_open) {
                fs->close(fd);
            }
        }
        for (auto& [fd, info] : open_dirs) {
            if (info.is_open) {
                fs->closedir(fd);
            }
        }

        // Print statistics
        std::cout << "\n=== Test Statistics ===" << std::endl;
        std::cout << "Total directories created: " << total_directories_created
                  << std::endl;
        std::cout << "Total files created: " << total_files_created
                  << std::endl;
        std::cout << "Total bytes written: " << total_bytes_written
                  << std::endl;
        std::cout << "Total operations: " << total_operations << std::endl;
        std::cout << "Final filesystem state:" << std::endl;
        std::cout << "  Directories: " << directory_paths.size() << std::endl;
        std::cout << "  Files: " << file_paths.size() << std::endl;
        std::cout << "  Total paths: " << existing_paths.size() << std::endl;
    }

    // Helper functions
    std::string generateRandomString(size_t length) {
        const std::string chars =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<> char_dist(0, chars.size() - 1);
        std::string result;
        for (size_t i = 0; i < length; ++i) {
            result += chars[char_dist(rng)];
        }
        return result;
    }

    std::string generateRandomContent(size_t size) {
        std::string content;
        content.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            content += static_cast<char>('A' + (i % 26));
        }
        return content;
    }

    std::vector<std::string> pathToVector(const std::string& path) {
        std::vector<std::string> result;
        if (path.empty()) return result;

        std::istringstream iss(path);
        std::string component;
        while (std::getline(iss, component, '/')) {
            if (!component.empty()) {
                result.push_back(component);
            }
        }
        return result;
    }

    std::string vectorToPath(const std::vector<std::string>& components) {
        if (components.empty()) return "";
        std::string result;
        for (const auto& comp : components) {
            result += "/" + comp;
        }
        return result.substr(1);  // Remove leading slash for our path format
    }

    std::string getParentPath(const std::string& path) {
        auto components = pathToVector(path);
        if (components.empty()) return "";
        components.pop_back();
        return vectorToPath(components);
    }

    std::string getBasename(const std::string& path) {
        auto components = pathToVector(path);
        return components.empty() ? "" : components.back();
    }

    // Verification functions
    void verifyPathExists(const std::string& path) {
        EXPECT_TRUE(fs->exists(pathToVector(path)));
        EXPECT_TRUE(existing_paths.count(path));
    }

    void verifyPathNotExists(const std::string& path) {
        EXPECT_FALSE(fs->exists(pathToVector(path)));
        EXPECT_FALSE(existing_paths.count(path));
    }

    void verifyIsFile(const std::string& path) {
        EXPECT_TRUE(fs->is_file(pathToVector(path)));
        EXPECT_TRUE(file_paths.count(path));
        EXPECT_FALSE(directory_paths.count(path));
    }

    void verifyIsDirectory(const std::string& path) {
        EXPECT_TRUE(fs->is_directory(pathToVector(path)));
        EXPECT_TRUE(directory_paths.count(path));
        EXPECT_FALSE(file_paths.count(path));
    }

    void verifyFileSize(const std::string& path, size_t expected_size) {
        auto actual_size = fs->filesize(pathToVector(path));
        EXPECT_TRUE(actual_size.has_value());
        if (actual_size.has_value()) {
            EXPECT_EQ(actual_size.value(), expected_size);
        }
        EXPECT_EQ(file_sizes[path], expected_size);
    }

    void verifyFileContent(const std::string& path,
                           const std::string& expected_content) {
        auto fd = fs->open(pathToVector(path), fileopenmode::READ);
        EXPECT_TRUE(fd.has_value());

        if (fd.has_value()) {
            std::string actual_content;
            actual_content.resize(expected_content.size());
            size_t bytes_read = fs->read(fd.value(), actual_content.data(),
                                         actual_content.size());
            EXPECT_EQ(bytes_read, expected_content.size());
            EXPECT_EQ(actual_content, expected_content);
            fs->close(fd.value());
        }
    }

    void verifyDirectoryListing(const std::string& path) {
        auto entries = fs->listdir(pathToVector(path));

        // Count expected children
        size_t expected_count = 0;
        for (const auto& existing_path : existing_paths) {
            if (getParentPath(existing_path) == path) {
                expected_count++;
            }
        }

        EXPECT_EQ(entries.size(), expected_count);

        // Verify each entry
        for (const auto& entry : entries) {
            std::string child_path = path.empty()
                                         ? entry.m_path.back()
                                         : path + "/" + entry.m_path.back();
            EXPECT_TRUE(existing_paths.count(child_path));

            if (entry.m_type == EntryType::UserStorage) {
                EXPECT_TRUE(directory_paths.count(child_path));
            } else if (entry.m_type == EntryType::UserStream) {
                EXPECT_TRUE(file_paths.count(child_path));
                EXPECT_EQ(entry.m_size, file_sizes[child_path]);
            }
        }
    }

    void verifyFilesystemIntegrity() {
        // Verify all tracked paths exist in filesystem
        for (const auto& path : existing_paths) {
            verifyPathExists(path);

            if (directory_paths.count(path)) {
                verifyIsDirectory(path);
                verifyDirectoryListing(path);
            } else if (file_paths.count(path)) {
                verifyIsFile(path);
                verifyFileSize(path, file_sizes[path]);
                if (file_contents.count(path)) {
                    verifyFileContent(path, file_contents[path]);
                }
            }
        }

        // Verify no extra paths exist
        std::function<void(const std::string&)> checkNoExtraEntries =
            [&](const std::string& dir_path) {
                auto entries = fs->listdir(pathToVector(dir_path));
                for (const auto& entry : entries) {
                    std::string child_path =
                        dir_path.empty() ? entry.m_path.back()
                                         : dir_path + "/" + entry.m_path.back();
                    EXPECT_TRUE(existing_paths.count(child_path));

                    if (entry.m_type == EntryType::UserStorage) {
                        checkNoExtraEntries(child_path);
                    }
                }
            };

        checkNoExtraEntries("");
    }

    // Test operations
    void createDirectory(const std::string& path) {
        auto components = pathToVector(path);
        bool success = fs->mkdir(components);

        if (success) {
            existing_paths.insert(path);
            directory_paths.insert(path);
            total_directories_created++;

            // Ensure parent directories exist in tracking
            std::string parent = getParentPath(path);
            if (!parent.empty()) {
                EXPECT_TRUE(directory_paths.count(parent));
            }
        }

        total_operations++;
    }

    void createFile(const std::string& path, const std::string& content) {
        auto components = pathToVector(path);
        auto fd =
            fs->open(components, fileopenmode::CREATE | fileopenmode::WRITE);

        if (fd.has_value()) {
            size_t bytes_written =
                fs->write(fd.value(), content.data(), content.size());
            fs->close(fd.value());

            if (bytes_written == content.size()) {
                existing_paths.insert(path);
                file_paths.insert(path);
                file_contents[path] = content;
                file_sizes[path] = content.size();
                total_files_created++;
                total_bytes_written += bytes_written;
            }
        }

        total_operations++;
    }

    void writeToFile(const std::string& path, const std::string& content,
                     bool append = false) {
        auto components = pathToVector(path);
        fileopenmode mode = append ? static_cast<fileopenmode>(
                                         static_cast<int>(fileopenmode::WRITE) |
                                         static_cast<int>(fileopenmode::APPEND))
                                   : fileopenmode::WRITE;
        auto fd = fs->open(components, mode);

        if (fd.has_value()) {
            size_t bytes_written =
                fs->write(fd.value(), content.data(), content.size());
            fs->close(fd.value());

            if (bytes_written == content.size()) {
                if (append) {
                    file_contents[path] += content;
                } else {
                    file_contents[path] = content;
                }
                file_sizes[path] = file_contents[path].size();
                total_bytes_written += bytes_written;
            }
        }

        total_operations++;
    }

    void removeFile(const std::string& path) {
        auto components = pathToVector(path);
        bool success = fs->unlink(components);

        if (success) {
            existing_paths.erase(path);
            file_paths.erase(path);
            file_contents.erase(path);
            file_sizes.erase(path);
        }

        total_operations++;
    }

    void removeDirectory(const std::string& path) {
        auto components = pathToVector(path);
        bool success = fs->rmdir(components);

        if (success) {
            existing_paths.erase(path);
            directory_paths.erase(path);
        }

        total_operations++;
    }

    void moveFileOrDirectory(const std::string& from, const std::string& to) {
        auto from_components = pathToVector(from);
        auto to_components = pathToVector(to);
        bool success = fs->move(from_components, to_components);

        if (success) {
            // Update tracking data structures
            existing_paths.erase(from);
            existing_paths.insert(to);

            if (directory_paths.count(from)) {
                directory_paths.erase(from);
                directory_paths.insert(to);

                // Update all child paths
                std::vector<std::string> to_update;
                for (const auto& path : existing_paths) {
                    if (path.substr(0, from.length()) == from &&
                        (path.length() == from.length() ||
                         path[from.length()] == '/')) {
                        to_update.push_back(path);
                    }
                }

                for (const auto& old_path : to_update) {
                    std::string new_path = to + old_path.substr(from.length());
                    existing_paths.erase(old_path);
                    existing_paths.insert(new_path);

                    if (directory_paths.count(old_path)) {
                        directory_paths.erase(old_path);
                        directory_paths.insert(new_path);
                    }
                    if (file_paths.count(old_path)) {
                        file_paths.erase(old_path);
                        file_paths.insert(new_path);

                        // Update file tracking
                        file_contents[new_path] = file_contents[old_path];
                        file_sizes[new_path] = file_sizes[old_path];
                        file_contents.erase(old_path);
                        file_sizes.erase(old_path);
                    }
                }
            } else if (file_paths.count(from)) {
                file_paths.erase(from);
                file_paths.insert(to);

                // Update file tracking
                file_contents[to] = file_contents[from];
                file_sizes[to] = file_sizes[from];
                file_contents.erase(from);
                file_sizes.erase(from);
            }
        }

        total_operations++;
    }

    void copyFile(const std::string& from, const std::string& to) {
        auto from_components = pathToVector(from);
        auto to_components = pathToVector(to);
        bool success = fs->copy(from_components, to_components);

        if (success) {
            existing_paths.insert(to);
            file_paths.insert(to);
            file_contents[to] = file_contents[from];
            file_sizes[to] = file_sizes[from];
        }

        total_operations++;
    }

    void testFileOperations(const std::string& path) {
        auto components = pathToVector(path);

        // Test different open modes
        auto fd_read = fs->open(components, fileopenmode::READ);
        EXPECT_TRUE(fd_read.has_value());

        if (fd_read.has_value()) {
            // Test tell and seek
            auto pos = fs->tell(fd_read.value());
            EXPECT_TRUE(pos.has_value());
            EXPECT_EQ(pos.value(), 0);

            // Test eof
            if (file_sizes[path] == 0) {
                EXPECT_TRUE(fs->eof(fd_read.value()));
            }

            // Test seek
            if (file_sizes[path] > 0) {
                bool seek_result = fs->seek(fd_read.value(), file_sizes[path],
                                            seekwhence::SET);
                EXPECT_TRUE(seek_result);
                EXPECT_TRUE(fs->eof(fd_read.value()));

                // Seek back to beginning
                fs->seek(fd_read.value(), 0, seekwhence::SET);
                EXPECT_FALSE(fs->eof(fd_read.value()));
            }

            // Test flush
            EXPECT_TRUE(fs->flush(fd_read.value()));

            fs->close(fd_read.value());
        }

        total_operations += 5;
    }

public:
    void runTest() {
        std::cout << "Starting filesystem test..." << std::endl;

        // Phase 1: Create deep directory structure
        std::cout << "Phase 1: Creating deep directory structure..."
                  << std::endl;
        createDeepDirectoryStructure();

        // Phase 2: Create many files at various levels
        std::cout << "Phase 2: Creating files at various levels..."
                  << std::endl;
        createManyFiles();

        // Phase 3: Test file operations extensively
        std::cout << "Phase 3: Testing file operations..." << std::endl;
        testAllFileOperations();

        // Phase 4: Test directory operations
        std::cout << "Phase 4: Testing directory operations..." << std::endl;
        testDirectoryOperations();

        // Phase 5: Test file and directory manipulation
        std::cout << "Phase 5: Testing file and directory manipulation..."
                  << std::endl;
        testFileAndDirectoryManipulation();

        // Phase 6: Test all utility functions
        std::cout << "Phase 6: Testing utility functions..." << std::endl;
        testUtilityFunctions();

        // Phase 7: Stress test with random operations
        std::cout << "Phase 7: Stress testing with random operations..."
                  << std::endl;
        stressTestWithRandomOperations();

        // Phase 8: Final verification
        std::cout << "Phase 8: Final verification..." << std::endl;
        verifyFilesystemIntegrity();

        std::cout << "test completed successfully!" << std::endl;
    }
    void createDeepDirectoryStructure() {
        // Create directories with depth up to MAX_DIRECTORY_DEPTH
        for (int depth = 1; depth <= MAX_DIRECTORY_DEPTH; ++depth) {
            std::string path;

            for (int level = 0; level < depth; ++level) {
                std::string dir_name = "level" + std::to_string(level) +
                                       "_depth" + std::to_string(depth);
                path = path.empty() ? dir_name : path + "/" + dir_name;

                if (!existing_paths.count(path)) {
                    createDirectory(path);
                }
            }

            // Create multiple branches at each depth (reduced)
            for (int branch = 0; branch < 2; ++branch) {
                std::string branch_path =
                    path + "/branch" + std::to_string(branch);
                createDirectory(branch_path);

                // Create sub-branches (reduced)
                for (int sub = 0; sub < 1; ++sub) {
                    std::string sub_path =
                        branch_path + "/sub" + std::to_string(sub);
                    createDirectory(sub_path);
                }
            }
        }

        // Create additional directories to reach minimum count
        while (total_directories_created < MIN_DIRECTORIES) {
            std::string path =
                "extra_dir_" + std::to_string(total_directories_created);
            createDirectory(path);
        }
    }

    void createManyFiles() {
        std::vector<std::string> target_dirs(directory_paths.begin(),
                                             directory_paths.end());

        // Create files in each directory (reduced count)
        for (const auto& dir : target_dirs) {
            // Create different types of files (reduced from 5 to 2)
            for (int i = 0; i < 2; ++i) {
                std::string filename = "file_" + std::to_string(i) + ".txt";
                std::string path =
                    dir.empty() ? filename : dir + "/" + filename;

                size_t content_size = size_dist(rng);
                std::string content = generateRandomContent(content_size);
                createFile(path, content);
            }
        }

        // Create additional files to reach minimum count
        while (total_files_created < MIN_FILES) {
            std::string path =
                "extra_file_" + std::to_string(total_files_created) + ".txt";
            std::string content = generateRandomContent(size_dist(rng));
            createFile(path, content);
        }
    }

    void testAllFileOperations() {
        std::vector<std::string> test_files(file_paths.begin(),
                                            file_paths.end());

        // Test basic file operations on all files
        for (const auto& path : test_files) {
            testFileOperations(path);
        }

        // Test file modification
        for (size_t i = 0; i < std::min(test_files.size(), size_t(20)); ++i) {
            const auto& path = test_files[i];

            // Test append
            std::string append_content =
                "\nAppended content " + std::to_string(i);
            writeToFile(path, append_content, true);

            // Test overwrite
            std::string new_content =
                "Overwritten content " + std::to_string(i);
            writeToFile(path, new_content, false);

            // Test truncate
            auto components = pathToVector(path);
            auto fd = fs->open(components, fileopenmode::WRITE);
            if (fd.has_value()) {
                size_t new_size = file_sizes[path] / 2;
                fs->truncate(fd.value(), new_size);
                fs->close(fd.value());

                file_contents[path] = file_contents[path].substr(0, new_size);
                file_sizes[path] = new_size;
            }
        }
    }

    void testDirectoryOperations() {
        std::vector<std::string> test_dirs(directory_paths.begin(),
                                           directory_paths.end());

        // Test directory listing on all directories
        for (const auto& dir : test_dirs) {
            verifyDirectoryListing(dir);

            // Test opendir/closedir
            auto dir_fd = fs->opendir(pathToVector(dir));
            EXPECT_TRUE(dir_fd.has_value());

            if (dir_fd.has_value()) {
                EXPECT_TRUE(fs->closedir(dir_fd.value()));
            }
        }
    }

    void testFileAndDirectoryManipulation() {
        // Test file copying
        std::vector<std::string> test_files(file_paths.begin(),
                                            file_paths.end());
        for (size_t i = 0; i < std::min(test_files.size(), size_t(10)); ++i) {
            std::string src = test_files[i];
            std::string dst = src + ".copy";
            copyFile(src, dst);
        }

        // Test file moving
        std::vector<std::string> movable_files;
        for (const auto& path : file_paths) {
            if (path.find(".copy") != std::string::npos) {
                movable_files.push_back(path);
            }
        }

        for (size_t i = 0; i < std::min(movable_files.size(), size_t(5)); ++i) {
            std::string src = movable_files[i];
            std::string dst = src + ".moved";
            moveFileOrDirectory(src, dst);
        }

        // Test directory moving (create temporary directories for this)
        for (int i = 0; i < 3; ++i) {
            std::string temp_dir = "temp_dir_" + std::to_string(i);
            createDirectory(temp_dir);

            std::string moved_dir = temp_dir + "_moved";
            moveFileOrDirectory(temp_dir, moved_dir);
        }
    }

    void testUtilityFunctions() {
        // Test all utility functions on all paths
        for (const auto& path : existing_paths) {
            auto components = pathToVector(path);

            // Test exists
            EXPECT_TRUE(fs->exists(components));

            // Test is_file and is_directory
            if (file_paths.count(path)) {
                EXPECT_TRUE(fs->is_file(components));
                EXPECT_FALSE(fs->is_directory(components));

                // Test filesize
                auto size = fs->filesize(components);
                EXPECT_TRUE(size.has_value());
            } else if (directory_paths.count(path)) {
                EXPECT_FALSE(fs->is_file(components));
                EXPECT_TRUE(fs->is_directory(components));
            }
        }

        // Test touch (create new empty files)
        for (int i = 0; i < 5; ++i) {
            std::string path = "touched_file_" + std::to_string(i) + ".txt";
            auto components = pathToVector(path);

            EXPECT_TRUE(fs->touch(components));

            existing_paths.insert(path);
            file_paths.insert(path);
            file_contents[path] = "";
            file_sizes[path] = 0;
        }

        // Test sync
        fs->sync();
    }

    void stressTestWithRandomOperations() {
        const size_t num_operations = 50;  // Reduced from 100

        for (size_t i = 0; i < num_operations; ++i) {
            int op = operation_dist(rng);

            if (op < 20) {
                // Create file
                std::string path = "stress_file_" + std::to_string(i) + ".txt";
                std::string content = generateRandomContent(size_dist(rng));
                createFile(path, content);

            } else if (op < 35) {
                // Create directory
                std::string path = "stress_dir_" + std::to_string(i);
                createDirectory(path);

            } else if (op < 50 && !file_paths.empty()) {
                // Modify existing file
                std::vector<std::string> files(file_paths.begin(),
                                               file_paths.end());
                std::uniform_int_distribution<> file_dist(0, files.size() - 1);
                std::string path = files[file_dist(rng)];

                std::string content = generateRandomContent(size_dist(rng));
                writeToFile(path, content,
                            rng() % 2 == 0);  // Random append/overwrite

            } else if (op < 60 && file_paths.size() > 10) {
                // Remove file
                std::vector<std::string> files(file_paths.begin(),
                                               file_paths.end());
                std::uniform_int_distribution<> file_dist(0, files.size() - 1);
                std::string path = files[file_dist(rng)];
                removeFile(path);

            } else if (op < 70 && !file_paths.empty()) {
                // Copy file
                std::vector<std::string> files(file_paths.begin(),
                                               file_paths.end());
                std::uniform_int_distribution<> file_dist(0, files.size() - 1);
                std::string src = files[file_dist(rng)];
                std::string dst = src + ".stress_copy_" + std::to_string(i);
                copyFile(src, dst);

            } else if (op < 80 && !file_paths.empty()) {
                // Move file
                std::vector<std::string> files(file_paths.begin(),
                                               file_paths.end());
                std::uniform_int_distribution<> file_dist(0, files.size() - 1);
                std::string src = files[file_dist(rng)];
                std::string dst = src + ".stress_moved_" + std::to_string(i);
                moveFileOrDirectory(src, dst);

            } else if (op < 90 && !file_paths.empty()) {
                // Test file operations
                std::vector<std::string> files(file_paths.begin(),
                                               file_paths.end());
                std::uniform_int_distribution<> file_dist(0, files.size() - 1);
                std::string path = files[file_dist(rng)];
                testFileOperations(path);

            } else {
                // Verify random paths
                std::vector<std::string> paths(existing_paths.begin(),
                                               existing_paths.end());
                if (!paths.empty()) {
                    std::uniform_int_distribution<> path_dist(0,
                                                              paths.size() - 1);
                    std::string path = paths[path_dist(rng)];
                    verifyPathExists(path);
                }
            }

            // Periodic integrity checks
            if (i % 25 == 0) {
                verifyFilesystemIntegrity();
            }
        }
    }
};

TEST(FS, filesystem_basic) {
    QRFilesystemTest test;
    test.SetUp();
    test.runTest();
    test.TearDown();
}

TEST(FS, deep_directory) {
    QRFilesystemTest test;
    test.SetUp();
    test.createDeepDirectoryStructure();
    EXPECT_GE(test.total_directories_created, test.MIN_DIRECTORIES);
    test.verifyFilesystemIntegrity();
    test.TearDown();
}

TEST(FS, many_files) {
    QRFilesystemTest test;
    test.SetUp();
    test.createDeepDirectoryStructure();
    test.createManyFiles();
    EXPECT_GE(test.total_files_created, test.MIN_FILES);
    test.verifyFilesystemIntegrity();
    test.TearDown();
}

TEST(FS, file_operations) {
    QRFilesystemTest test;
    test.SetUp();
    test.createDeepDirectoryStructure();
    test.createManyFiles();
    test.testAllFileOperations();
    test.verifyFilesystemIntegrity();
    test.TearDown();
}

TEST(FS, directory_operations) {
    QRFilesystemTest test;
    test.SetUp();
    test.createDeepDirectoryStructure();
    test.createManyFiles();
    test.testDirectoryOperations();
    test.verifyFilesystemIntegrity();
    test.TearDown();
}

TEST(FS, utility_functions) {
    QRFilesystemTest test;
    test.SetUp();
    test.createDeepDirectoryStructure();
    test.createManyFiles();
    test.testUtilityFunctions();
    test.verifyFilesystemIntegrity();
    test.TearDown();
}

TEST(FS, stress_operations) {
    QRFilesystemTest test;
    test.SetUp();
    test.createDeepDirectoryStructure();
    test.createManyFiles();
    test.stressTestWithRandomOperations();
    test.verifyFilesystemIntegrity();
    test.TearDown();
}
