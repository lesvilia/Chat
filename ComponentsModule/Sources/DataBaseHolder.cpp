
#include "DataBaseHolder.h"
#include "SQLite/sqlite3.h"

namespace db
{
  DataBaseHolder::DataBaseHolder(const std::string& filename)
    : m_fileName(filename)
    , m_db(nullptr)
    , m_error(SQLITE_OK)
  {
  }

  bool DataBaseHolder::Open()
  {
    m_error = sqlite3_open(m_fileName.c_str(), &m_db);
    if (m_error)
    {
      sqlite3_close(m_db);
    }
    return m_error == SQLITE_OK;
  }

  int DataBaseHolder::GetError() const
  {
    return m_error;
  }

  sqlite3* DataBaseHolder::GetHandle() const
  {
    return m_db;
  }

  DataBaseHolder::~DataBaseHolder()
  {
    if (m_db)
    {
      sqlite3_close(m_db);
    }
  }
}