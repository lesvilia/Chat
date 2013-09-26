#pragma once
#include <string>
#include "boost/noncopyable.hpp"

struct sqlite3;

namespace db
{
  class DataBaseHolder
    : private boost::noncopyable
  {
  public:
    explicit DataBaseHolder(const std::string& filename);
    bool Open();
    int GetError() const;
    sqlite3* GetHandle() const;
    ~DataBaseHolder();
  private:
    std::string m_fileName;
    sqlite3* m_db;
    int m_error;
  };
}