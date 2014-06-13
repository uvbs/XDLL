
#pragma once



#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")



inline void CheckComResult(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_issue_error(hr);
	}
}


class AdoConnection
{
public:
	AdoConnection()
	{
		HRESULT hr = m_conn.CreateInstance(__uuidof(Connection));
		CheckComResult(hr);
	}
	~AdoConnection()
	{
		Close();
	}

	_ConnectionPtr& GetComPtr()
	{
		return m_conn;
	}
	bool IsValid() const
	{
		return m_conn;
	}
	bool IsOpen() const
	{
		return IsValid() && m_conn->State == adStateOpen;
	}
	void Close()
	{
		if (IsOpen())
		{
			m_conn->Close();
		}
	}


	void OpenJet(const char* datasrc, const char* username = "", const char* password = "")
	{
		char str[1024] = { 0 };
		sprintf(str, "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;", datasrc);
		m_conn->Open(_bstr_t(str), username, password, adConnectUnspecified);
	}

	variant_t Execute(const char* sqlText, long options = adCmdUnspecified | adOptionUnspecified)
	{
		variant_t res;
		_RecordsetPtr ptr = m_conn->Execute(sqlText, &res, options);
		assert(ptr == NULL);
		return res;
	}
	_RecordsetPtr Query(const char* sqlText, long options = adCmdUnspecified | adOptionUnspecified)
	{
		variant_t res;
		return m_conn->Execute(sqlText, &res, options);
	}
	_RecordsetPtr OpenSchema(SchemaEnum queryType)
	{
		return m_conn->OpenSchema(queryType);
	}

private:
	_ConnectionPtr	m_conn;
};


class AdoDataTable
{
public:
	explicit AdoDataTable(_RecordsetPtr& rst) : m_rst(rst)
	{
		assert(m_rst != NULL);
	}
	AdoDataTable(AdoConnection& conn, const char* sqlText)
	{
		m_rst = conn.Query(sqlText);
	}
	AdoDataTable()
	{
		HRESULT hr = m_rst.CreateInstance(__uuidof(Recordset));
		CheckComResult(hr);
	}
	~AdoDataTable()
	{
		Close();
	}

	void Attach(AdoConnection& conn)
	{
		m_rst->PutRefActiveConnection(conn.GetComPtr());
	}

	bool IsValid() const
	{
		return m_rst;
	}
	bool IsOpen() const
	{
		return IsValid() && m_rst->State == adStateOpen;
	}
	void Close()
	{
		if (IsOpen())
		{
			m_rst->Close();
		}
	}

	void Query(const char* sqlText, AdoConnection& conn)
	{
		Close();
		m_rst->Open(_variant_t(sqlText), _variant_t(conn.GetComPtr().GetInterfacePtr()), adOpenUnspecified, adLockUnspecified, adCmdUnspecified | adOptionUnspecified);
	}

	bool HasData() const
	{
		return !m_rst->adoEOF;
	}
	void MoveNext()
	{
		assert(HasData());
		m_rst->MoveNext();
	}
	_bstr_t GetString(const char* index)
	{
		variant_t res = GetField(_variant_t(index));
		if (res.vt == VT_NULL)
			return "";
		assert(res.vt == VT_BSTR);
		return res;
	}
	long GetInteger(const char* index)
	{
		variant_t res = GetField(_variant_t(index));
		assert(res.vt == VT_I4);
		return res;
	}
	double GetReal(const char* index)
	{
		variant_t res = GetField(_variant_t(index));
		assert(res.vt == VT_R8);
		return res;
	}
	_bstr_t GetString(long index)
	{
		variant_t res = GetField(index);
		assert(res.vt == VT_BSTR);
		return res;
	}
	long GetInteger(long index)
	{
		variant_t res = GetField(index);
		assert(res.vt == VT_I4);
		return res;
	}
	double GetReal(long index)
	{
		variant_t res = GetField(index);
		assert(res.vt == VT_R8);
		return res;
	}
	_variant_t GetField(const variant_t& index)
	{
		assert(HasData());
		return m_rst->Fields->Item[index]->Value;
	}

	_RecordsetPtr	m_rst;
};



