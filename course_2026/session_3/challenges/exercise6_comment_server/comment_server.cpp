#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace {

struct Comment {
    size_t size;
    char *data;
};

bool read_payload(std::istream &in, size_t size, char *dst) {
    in.read(dst, static_cast<std::streamsize>(size));
    return static_cast<size_t>(in.gcount()) == size;
}

void submit(std::istream &in, std::map<std::string, Comment> &comments, const std::string &user, size_t size) {
    char *buf = new char[size + 1];
    if (!read_payload(in, size, buf)) {
        delete[] buf;
        return;
    }
    buf[size] = '\0';

    auto it = comments.find(user);
    if (it != comments.end()) {
        delete[] it->second.data;
    }
    comments[user] = Comment{size, buf};
}

void replace(std::istream &in, std::map<std::string, Comment> &comments, const std::string &user, size_t size) {
    auto it = comments.find(user);
    if (it == comments.end()) {
        return;
    }

    // Bug: the old comment is freed before the replacement is known to be
    // complete, and the map still keeps the dangling pointer on short input.
    delete[] it->second.data;

    char *buf = new char[size + 1];
    if (!read_payload(in, size, buf)) {
        delete[] buf;
        return;
    }

    buf[size] = '\0';
    it->second = Comment{size, buf};
}

void erase(std::map<std::string, Comment> &comments, const std::string &user) {
    auto it = comments.find(user);
    if (it == comments.end()) {
        return;
    }
    delete[] it->second.data;
    comments.erase(it);
}

void view(const std::map<std::string, Comment> &comments, const std::string &user) {
    auto it = comments.find(user);
    if (it == comments.end()) {
        return;
    }
    volatile char first = it->second.data[0];
    (void)first;
}

void parse(std::istream &in) {
    std::map<std::string, Comment> comments;
    std::string line;

    while (std::getline(in, line)) {
        std::istringstream cmd(line);
        std::string op;
        std::string user;
        size_t size = 0;
        cmd >> op >> user >> size;

        if (op == "SUBMIT") {
            submit(in, comments, user, size);
        } else if (op == "REPLACE") {
            replace(in, comments, user, size);
        } else if (op == "DELETE") {
            erase(comments, user);
        } else if (op == "VIEW") {
            view(comments, user);
        }
    }

    for (auto &entry : comments) {
        delete[] entry.second.data;
    }
}

}  // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        return 2;
    }
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        return 1;
    }
    parse(in);
    return 0;
}
