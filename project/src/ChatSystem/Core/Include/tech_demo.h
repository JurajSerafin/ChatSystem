#ifndef TECH_DEMO_H
#define TECH_DEMO_H

#include <string>


namespace TechDemo
{
    void print_separator(const std::string& title);

    void test_wxwidgets();

    void test_boost();

    void test_openssl();

    void test_postgresql();

    void test_sqlite();

    void test_json();

    void test_spdlog();

    void run_all();
};

#endif