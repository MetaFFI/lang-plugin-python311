#include <doctest/doctest.h>

#include "python3_test_env.h"
#include "python3_wrappers.h"

#include <string>
#include <vector>

#ifndef DOCTEST_SKIP
#define DOCTEST_SKIP(msg)           \
	do                            \
	{                             \
		DOCTEST_INFO(msg);        \
		return;                   \
	} while(false)
#endif

TEST_CASE("third party modules")
{
	auto& env = python_test_env();

	// bs4
	try
	{
		metaffi::api::MetaFFIModule bs4(env.runtime.runtime_plugin(), "bs4");
		auto soup_ctor = bs4.load_entity(
			"callable=BeautifulSoup",
			{metaffi_string8_type, metaffi_string8_type},
			{metaffi_handle_type});
		auto [soup_ptr] = soup_ctor.call<cdt_metaffi_handle*>(
			std::string("<html><body><p>hi</p></body></html>"),
			std::string("html.parser"));
		PyHandle soup_handle(soup_ptr);

		auto getattr_handle = env.builtins.load_entity(
			"callable=getattr",
			{metaffi_handle_type, metaffi_string8_type},
			{metaffi_handle_type});
		auto getattr_string = env.builtins.load_entity(
			"callable=getattr",
			{metaffi_handle_type, metaffi_string8_type},
			{metaffi_string8_type});

		auto [body_ptr] = getattr_handle.call<cdt_metaffi_handle*>(*soup_handle.get(), std::string("body"));
		PyHandle body_handle(body_ptr);
		auto [p_ptr] = getattr_handle.call<cdt_metaffi_handle*>(*body_handle.get(), std::string("p"));
		PyHandle p_handle(p_ptr);
		auto [text] = getattr_string.call<std::string>(*p_handle.get(), std::string("text"));
		CHECK(text == "hi");
	}
	catch(const std::exception& ex)
	{
		if(std::string(ex.what()).find("No module named") != std::string::npos)
		{
			DOCTEST_SKIP("bs4 not installed");
		}
		throw;
	}

	// numpy
	try
	{
		metaffi::api::MetaFFIModule np(env.runtime.runtime_plugin(), "numpy");
		auto array_fn = np.load_entity(
			"callable=array",
			{metaffi_int64_array_type},
			{metaffi_handle_type});
		std::vector<std::vector<int64_t>> matrix = {{1, 2}, {3, 4}};
		auto [arr_ptr] = array_fn.call<cdt_metaffi_handle*>(matrix);
		PyHandle arr_handle(arr_ptr);

		auto sum_fn = np.load_entity(
			"callable=sum",
			{metaffi_handle_type},
			{metaffi_handle_type});
		auto [sum_ptr] = sum_fn.call<cdt_metaffi_handle*>(*arr_handle.get());
		PyHandle sum_handle(sum_ptr);

		auto int_entity = env.builtins.load_entity(
			"callable=int",
			{metaffi_handle_type},
			{metaffi_int64_type});
		auto [sum_val] = int_entity.call<int64_t>(*sum_handle.get());
		CHECK(sum_val == 10);
	}
	catch(const std::exception& ex)
	{
		if(std::string(ex.what()).find("No module named") != std::string::npos)
		{
			DOCTEST_SKIP("numpy not installed");
		}
		throw;
	}

	// pandas
	try
	{
		metaffi::api::MetaFFIModule pd(env.runtime.runtime_plugin(), "pandas");
		auto df_ctor = pd.load_entity(
			"callable=DataFrame",
			{metaffi_handle_type},
			{metaffi_handle_type});

		auto data = PyDict::create();
		data.set(std::string("a"), std::vector<int64_t>{1, 2, 3});
		data.set(std::string("b"), std::vector<int64_t>{4, 5, 6});

		auto [df_ptr] = df_ctor.call<cdt_metaffi_handle*>(*data.handle());
		PyHandle df_handle(df_ptr);

		auto getitem = pd.load_entity(
			"callable=DataFrame.__getitem__,instance_required",
			{metaffi_handle_type, metaffi_string8_type},
			{metaffi_handle_type});
		auto [col_ptr] = getitem.call<cdt_metaffi_handle*>(*df_handle.get(), std::string("b"));
		PyHandle col_handle(col_ptr);

		auto sum_method = pd.load_entity(
			"callable=Series.sum,instance_required",
			{metaffi_handle_type},
			{metaffi_handle_type});
		auto [sum_ptr] = sum_method.call<cdt_metaffi_handle*>(*col_handle.get());
		PyHandle sum_handle(sum_ptr);

		auto int_entity = env.builtins.load_entity(
			"callable=int",
			{metaffi_handle_type},
			{metaffi_int64_type});
		auto [sum_val] = int_entity.call<int64_t>(*sum_handle.get());
		CHECK(sum_val == 15);
	}
	catch(const std::exception& ex)
	{
		if(std::string(ex.what()).find("No module named") != std::string::npos)
		{
			DOCTEST_SKIP("pandas not installed");
		}
		throw;
	}

	// requests
	try
	{
		metaffi::api::MetaFFIModule req(env.runtime.runtime_plugin(), "requests");
		auto urlparse = req.load_entity(
			"callable=utils.urlparse",
			{metaffi_string8_type},
			{metaffi_handle_type});
		auto [url_ptr] = urlparse.call<cdt_metaffi_handle*>(
			std::string("https://example.com/path?x=1"));
		PyHandle url_handle(url_ptr);

		auto getattr_string = env.builtins.load_entity(
			"callable=getattr",
			{metaffi_handle_type, metaffi_string8_type},
			{metaffi_string8_type});
		auto [scheme] = getattr_string.call<std::string>(*url_handle.get(), std::string("scheme"));
		CHECK(scheme == "https");
	}
	catch(const std::exception& ex)
	{
		if(std::string(ex.what()).find("No module named") != std::string::npos)
		{
			DOCTEST_SKIP("requests not installed");
		}
		throw;
	}
}
