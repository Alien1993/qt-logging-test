#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>
#include <QObject>
#include <QStandardPaths>

#include <fstream>
#include <iomanip>

Q_LOGGING_CATEGORY(mainlog, "main")

static std::fstream logFile;

void fileMessageHandler(QtMsgType type, const QMessageLogContext& context,
                        const QString& msg) {
  // Log timestamp
  logFile << QDateTime::currentMSecsSinceEpoch() << ' ';

  logFile << std::left << std::setfill(' ') << std::setw(12);
  switch (type) {
    case QtDebugMsg: {
      logFile << "[DEBUG]";
      break;
    }
    case QtInfoMsg: {
      logFile << "[INFO]";
      break;
    }
    case QtWarningMsg: {
      logFile << "[WARNING]";
      break;
    }
    case QtCriticalMsg: {
      logFile << "[CRITICAL]";
      break;
    }
    case QtFatalMsg: {
      logFile << "[FATAL]";
      break;
    }
  }
  logFile << std::left << std::setfill(' ') << std::setw(20);
  logFile << context.category;
  logFile << msg.toStdString();

// We have context informations only on debug builds
#ifdef QT_DEBUG
  logFile << ' ' << context.file << ':' << context.line << ':'
          << context.function;
#endif

  logFile << std::endl;
}

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);

  auto logFilePath = QCoreApplication::applicationDirPath() + "/" +
                     QCoreApplication::applicationName() + ".log";
  logFile.open(logFilePath.toStdString(),
               std::ios::out | std::ios::in | std::ios::app);

  qInstallMessageHandler(fileMessageHandler);

// Enables all debug log messages on debug build
#ifdef QT_DEBUG
  QLoggingCategory::setFilterRules(QStringLiteral("*.debug=true"));
#endif

  // Closes log file on quit
  QObject::connect(&a, &QCoreApplication::aboutToQuit, [] {
    logFile.flush();
    logFile.close();
  });

  // Some logs to show off
  qCInfo(mainlog) << "All's cool, just letting you know.";

  qCWarning(mainlog) << "Are you sure?";

  qCCritical(mainlog) << "You should take a look at this!";

  qFatal("🔥 SHIT'S ON FIRE! 🔥 PANIC! 🔥");

  return a.exec();
}
