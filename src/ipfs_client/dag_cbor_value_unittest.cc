#include <mock_dag_cbor_value.h>

TEST(DagCborValueTest,WeirdHtml) {
  MockCborVal list;
  auto& l = list.list;
  l.emplace_back();
  l.back().bol = true;
  l.emplace_back();
  l.back().flt = 9.2;
  l.emplace_back();
  l.back().number = -1;
  l.emplace_back();
  auto actual = list.html();
  EXPECT_EQ(actual,R"(<html><title>DAG-CBOR Preview</title><body>
<ol class='cbor_array'>
  <li>
 <b class='cbor_bool'>True</b>
  </li>
  <li>
<b class='cbor_float'>0</b>
  </li>
  <li>
<b class='cbor_signed'>-1</b>
  </li>
  <li>
<b class='cbor_null' style='color:red;'>&empty;</b>
  </li>
</ol>
</body></html>)");
}
