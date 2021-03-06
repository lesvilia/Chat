#pragma once

namespace msg
{
  const wchar_t MESSAGE_NODE[] = { L"message" };
  const wchar_t USER_NODE[] = { L"user" };
  const wchar_t USER_UUID_NODE[] = { L"uuid" };
  const wchar_t USER_NAME_NODE[] = { L"name" };
  const wchar_t DATA_NODE[] = { L"data" };
  const wchar_t FILE_SIZE_NODE[] = { L"filesize" };
  const wchar_t FILE_NAME_NODE[] = { L"filename" };

  const wchar_t STATE_MESSAGE_FORMAT[] = { L"<message><user><uuid>%1%</uuid><name>%2%</name></user><data>%3%</data></message>" };
  const wchar_t CHAT_MESSAGE_FORMAT[] = { L"<message><user><uuid>%1%</uuid></user><data>%2%</data></message>" };
  const wchar_t FILE_MESSAGE_HEADER[] = { L"<message><user><uuid>%1%</uuid></user><data>"
                                          L"<filesize>%2%</filesize><filename>%3%</filename></data></message>" };
}
