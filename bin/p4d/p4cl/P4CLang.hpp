#pragma once
#include <vector>
#include <variant>
#include <string>
#include <algorithm>

namespace p4cl {
namespace ast {
class Node;
}
struct TaggedPosition {
    static TaggedPosition fromStrView(std::string_view strView)
    {
        TaggedPosition p;
        p.strView = strView;
        return p;
    }

    void annotate(const char* begin, const char* end)
    {
        strView = std::string_view(begin, std::distance(begin, end));
    }

    bool isValidPosition() const
    {
        return strView.size() != 0;
    }

    size_t size() const
    {
        return strView.size();
    }

    std::string_view strView;
};

namespace lang {

enum class CompilationMode { Permissive, Strict };

namespace errors {
static const char* unknownDataType = "Unknown data type";
static const char* unknownTaskType = "Unknown task type";
static const char* invalidHierarchy = "Invalid hierarchy";
static const char* invalidSource = "Invalid source";
}

struct CompilationMessage {
    enum Category { Information, Warning, ParsingError, CompilationError, EvaluationError };

    Category category;
    TaggedPosition position;
    std::string text;
};

using CompilationLog = std::vector<CompilationMessage>;

////////////////////////////////////////////////////////////////////////////////////////

enum class Scope { Program, Script, Task, Data, Options };
enum class ProgramNodeType { Program, Object, Script, Assignement, Identifier, Expression };

enum class DataKind { Images, Gcps, Mtps, CameraModel, InputData, Options };

enum class TaskKind {
    /// processing
    Calibration,
    GenerateDensePointCloud,
    GenerateMesh,
    GenerateDsm,
    GenerateDtm,
    GenerateOrthomosaic,

    /// exports

    ExportPointCloud,
    ExportMesh,
    ExportDsm,
    ExportDtm,
    ExportOrthomosaic,
};

enum class TaskCategory { Processing, Export };

////////////////////////////////////////////////////////////////////////////////////////

struct ProcessingNode {
    Scope category;
    std::variant<TaskKind, DataKind> type;

    TaskKind taskType() const
    {
        return std::get<TaskKind>(type);
    }

    DataKind dataType() const
    {
        return std::get<DataKind>(type);
    }
};

inline bool operator==(const ProcessingNode& l, const ProcessingNode& r)
{
    return l.category == r.category && l.type == r.type;
}

struct Connection {
    ProcessingNode from;
    ProcessingNode to;
};

inline bool operator==(const Connection& l, const Connection& r)
{
    return l.from == r.from && l.to == r.to;
}

inline Connection operator>>(TaskKind f, TaskKind t)
{
    Connection c;
    c.from = {Scope::Task, f};
    c.to = {Scope::Task, t};
    return c;
}

inline Connection operator>>(DataKind f, TaskKind t)
{
    Connection c;
    c.from = {Scope::Data, f};
    c.to = {Scope::Task, t};
    return c;
}

////////////////////////////////////////////////////////////////////////////////////////

struct DataType {
    const char* label = nullptr;
    DataKind type;

    operator bool() const
    {
        return label != nullptr;
    }
};

struct TaskType {
    const char* label = nullptr;
    TaskKind type;

    TaskCategory category() const
    {
        switch (type) {
            case TaskKind::Calibration:
            case TaskKind::GenerateDensePointCloud:
            case TaskKind::GenerateMesh:
            case TaskKind::GenerateDsm:
            case TaskKind::GenerateDtm:
            case TaskKind::GenerateOrthomosaic: {
                return TaskCategory::Processing;
            }
            case TaskKind::ExportPointCloud:
            case TaskKind::ExportMesh:
            case TaskKind::ExportDsm:
            case TaskKind::ExportDtm:
            case TaskKind::ExportOrthomosaic: {
                return TaskCategory::Export;
            }
            default: break;
        }
    }

    operator bool() const
    {
        return label != nullptr;
    }
};

////////////////////////////////////////////////////////////////////////////////////////

static const char* programTypeName = "program_t";

// clang-format off

static const std::vector<DataType> supportedData =
{
    {"images_source_t", DataKind::Images},
    {"gcp_source_t", DataKind::Gcps},
    {"mtps_source_t", DataKind::Mtps},
    {"camera_model_t", DataKind::CameraModel},
    {"input_data_t", DataKind::InputData},
    {"options_t", DataKind::Options}
};

inline DataType findDataType(std::string_view label)
{
    for(auto & d : supportedData)
    {
        if(label == d.label)
            return d;
    }
    return {};
}

static const std::vector<TaskType> supportedTasks =
{
    {"calibration_t", TaskKind::Calibration},
    {"generate_dense_pc_t", TaskKind::GenerateDensePointCloud},
    {"generate_mesh_t", TaskKind::GenerateMesh},
    {"generate_dsm_t", TaskKind::GenerateDsm},
    {"generate_dtm_t", TaskKind::GenerateDtm},
    {"generate_orthomosaic_t", TaskKind::GenerateOrthomosaic},

    {"export_pc_t", TaskKind::ExportPointCloud},
    {"export_mesh_t", TaskKind::ExportMesh},
    {"export_dsm_t", TaskKind::ExportDsm},
    {"export_dtm_t", TaskKind::ExportDtm},
    {"export_orthomosaic_t", TaskKind::ExportOrthomosaic},
};

inline TaskType findTaskType(std::string_view label)
{
    for(auto & d : supportedTasks)
    {
        if(label == d.label)
            return d;
    }
    return {};
}

////////////////////////////////////////////////////////////////////////////////////////

static const std::vector<Connection> supportedConnections =
{
    /// data
    DataKind::Images >> TaskKind::Calibration,
    DataKind::Gcps >> TaskKind::Calibration,
    DataKind::Mtps >> TaskKind::Calibration,
    DataKind::InputData >> TaskKind::Calibration,

    /// tasks
    TaskKind::Calibration >> TaskKind::GenerateDensePointCloud,
    TaskKind::GenerateDensePointCloud >> TaskKind::GenerateMesh,
    TaskKind::GenerateDensePointCloud >> TaskKind::GenerateDsm,
    TaskKind::GenerateDsm >> TaskKind::GenerateDtm,
    TaskKind::GenerateDsm >> TaskKind::GenerateOrthomosaic,

    /// exports
    TaskKind::GenerateDensePointCloud >> TaskKind::ExportPointCloud ,
    TaskKind::GenerateMesh >> TaskKind::ExportMesh,
    TaskKind::GenerateDsm >> TaskKind::ExportDsm,
    TaskKind::GenerateDtm>> TaskKind::ExportDtm,
    TaskKind::GenerateOrthomosaic >> TaskKind::ExportOrthomosaic,

};

// clang-format on

inline bool isValidConnection(TaskKind f, TaskKind t)
{
    return std::find(supportedConnections.begin(), supportedConnections.end(), f >> t) !=
           supportedConnections.end();
}

inline bool isValidConnection(DataKind f, TaskKind t)
{
    return std::find(supportedConnections.begin(), supportedConnections.end(), f >> t) !=
           supportedConnections.end();
}

// clang-format off

////////////////////////////////////////////////////////////////////////////////////////

static const std::vector<std::string> scriptTypes = {
    "script_t",
    "on_started",
    "on_finished"
};

inline bool isScript(std::string_view label)
{
    return label == "script_t";
}

inline bool isScriptObject(std::string_view str)
{
    return std::find(scriptTypes.begin(),scriptTypes.end(), str) != scriptTypes.end();
}

////////////////////////////////////////////////////////////////////////////////////////

static const char* endKeyWords[] = {
    "end"
};
static const char* p4clKeyWords[] = {
    "override"
};

static const char* jsKeyWords[] = {
    "do",      "if",      "in",      "for",      "new",      "try",
    "var",     "case",    "else",    "enum",     "null",     "undefined",
    "this",    "true",    "void",    "with",     "break",    "catch",
    "class",   "const",   "false",   "super",    "throw",    "while",
    "delete",  "export",  "import",  "return",   "switch",   "typeof",
    "default", "extends", "finally", "continue", "debugger", "function"
};

// clang-format on

////////////////////////////////////////////////////////////////////////////////////////

struct PropertyInfo {
    const char* name = nullptr;
    const char* defaultValue;
    bool optional = false;

    operator bool() const
    {
        return name != nullptr;
    }
};

struct TypeProperties {
    const char* typeName = nullptr;
    std::vector<PropertyInfo> properties;
    operator bool() const
    {
        return typeName != nullptr;
    }
};

// clang-format off

static const TypeProperties typeProperties [] =
{
    {
        "script_t", {}
    },
    {
        "images_source_t", {
            {"location", "\"<path>\""},
            {"filter", "\"<pattern>\"", true}
        },
    },
    {
        "gcps_source_t", {
            {"source", "\"<path>\""},
        },
    },
    {
        "mtps_source_t", {
            {"source", "\"<path>\""},
        },
    },
    {
        "camera_model_t", {
            {"image_size", "[0, 0]"},
            {"pixel_size", "0.0"},
            {"principle_point_position", "[0.0, 0.0]"},
            {"focal_length", "0.0"},
            {"lens_type", "\"perspective\""},
            {"radial_k1", "0.0"},
            {"radial_k2", "0.0"},
            {"radial_k3", "0.0"},
            {"tangencial_t1", "0.0",},
            {"tangencial_t2", "0.0",},
        }
    },
    {
        "calibration_t", {
            {"source", "[]"},
        }
    },
    {
        "generate_dense_pc_t", {
            {"source", "[]"},
        }
    },
    {
        "generate_mesh_t", {
            {"source", "[]"},
        }
    },
    {
        "generate_dsm_t", {
            {"source", "[]"},
        }
    },
    {
        "generate_dtm_t", {
            {"source", "[]"},
        }
    },
    {
        "generate_orthomosaic_t", {
            {"source", "[]"},
        }
    },
    {
        "export_pc_t", {
            {"source", "[]"},
            {"format", "\"<format>\"", false}
        }
    },
    {
        "export_mesh_t", {
            {"source", "[]"},
            {"format", "\"<format>\"", false}
        }
    },
    {
        "export_dsm_t", {
            {"source", "[]"},
            {"format", "\"<format>\"", false}
        }
    },
    {
        "export_dtm_t", {
            {"source", "[]"},
            {"format", "\"<format>\"", false}
        }
    },
    {
        "export_orthomosaic_t", {
            {"source", "[]"},
            {"format", "\"<format>\"", false}
        }
    }

};
// clang-format on

inline TypeProperties findTypeProperties(std::string_view typeName)
{
    auto iter = std::find_if(std::begin(typeProperties), std::end(typeProperties),
                             [typeName](auto&& t) { return typeName == t.typeName; });

    if (iter != std::end(typeProperties)) return *iter;
    return {};
}

////////////////////////////////////////////////////////////////////////////////////////

struct ObjectNestingInfo {
    const char* typeName = nullptr;
    std::vector<const char*> subTypes;
    operator bool() const
    {
        return typeName != nullptr;
    }
};

// clang-format off
static const ObjectNestingInfo objectNestingInfo[] =
{
    { "images_source_t", {"camera_model_t"} },
    { "calibration_t", {"options_t"} },
    { "generate_dense_pc_t", {"options_t"} },
    { "generate_mesh_t", {"options_t"} },
    { "generate_dsm_t", {"options_t"} },
    { "generate_dtm_t", {"options_t"} },
    { "generate_orthomosaic_t", {"options_t"} },
    {
        lang::programTypeName,
        {
            "script_t",
            "images_source_t",
            "gcp_source_t",
            "mtps_source_t",
            "camera_model_t",
            "calibration_t",
            "generate_dense_pc_t",
            "generate_mesh_t",
            "generate_dsm_t",
            "generate_dtm_t",
            "generate_orthomosaic_t",
            "export_pc_t",
            "export_mesh_t",
            "export_dsm_t",
            "export_dtm_t",
            "export_orthomosaic_t",
        }
    }
};
// clang-format on

inline ObjectNestingInfo findTypeNestedTypes(std::string_view typeName)
{
    auto iter = std::find_if(std::begin(objectNestingInfo), std::end(objectNestingInfo),
                             [typeName](auto&& t) { return typeName == t.typeName; });

    if (iter != std::end(objectNestingInfo)) return *iter;
    return {};
}

}  /// namespace lang

////////////////////////////////////////////////////////////////////////////////////////

/// Helpers

inline const char* toString(lang::CompilationMessage::Category c)
{
    switch (c) {
        case lang::CompilationMessage::Information: return "Information";
        case lang::CompilationMessage::Warning: return "Warning";
        case lang::CompilationMessage::ParsingError: return "Parsing error";
        case lang::CompilationMessage::CompilationError: return "Compilation error";
        case lang::CompilationMessage::EvaluationError: return "Evaluation error"; break;
        default: return "Unknown";
    }
}

inline std::string toString(lang::CompilationMessage m)
{
    return toString(m.category) + (" : " + m.text);
}
}
