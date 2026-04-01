#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <vector>
#include <string>
#include "../model/Token.h"
#include "../errors/ErrorManager.h"

class ReportGenerator {
public:
    static void generateTokenReport(
        const std::vector<Token>& tokens,
        const std::string& filename
        );

    static void generateErrorReport(
        const std::vector<LexicalError>& errors,
        const std::string& filename
        );

    static void generatePatientReport(
        const std::vector<Token>& tokens,
        const std::string& filename
        );

    static void generateDoctorReport(
        const std::vector<Token>& tokens,
        const std::string& filename
        );

    static void generateAppointmentReport(
        const std::vector<Token>& tokens,
        const std::string& filename
        );

    static void generateStatsReport(
        const std::vector<Token>& tokens,
        const std::string& filename
        );

    static void generateGraphviz(
        const std::vector<Token>& tokens,
        const std::string& filename
        );
};

#endif