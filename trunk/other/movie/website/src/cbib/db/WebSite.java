package cbib.db;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;
import java.util.Vector;

/**
 * 要抓取的网站列表
 * @author hankjin
 *
 */
public class WebSite {
	public int id;		
	public String url;		//网址
	public String parser;	//Python解析器
	public String note;		//备注
	
	/**
	*创建对象
	*默认全部为空
	*/
	public WebSite(){
		id = 0;
		url = "";
		parser="DefaultParser";
		note = "";
	}	
	/**
	*更新站点信息到数据库
	*@return true:succeed; false: failed
	*/
	public boolean update2DB() throws SQLException{
		String sql = "update website set url=?, parser=?, note=?"
			+ " where id=?";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,url);				// 网址
		stmt.setString(2,parser);			// Python解析器
		stmt.setString(3,note);			// 备注
		stmt.setInt(4, id);
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	 * 根据ID删除站点
	 * @param id
	 * @return
	 * @throws SQLException
	 */
	public static boolean delWebSite(int id) throws SQLException{
		DBConn conn = new DBConn();
		Statement stmt = conn.getStmt();
		int result = stmt.executeUpdate("delete from website where id="+id);
		conn.close();
		return result > 0;		
	}
	/**
	*根据ID获取对象
	*/
	public static WebSite getWebSite(int id) throws SQLException{
		List<WebSite> result = getWebSites("Select * from website where id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	public static List<WebSite> getWebSites( ) throws SQLException{
		return getWebSites("select * from website"); 
	}
	/**
	*根据SQL获取对象
	*/	
	public static List<WebSite> getWebSites(String sql) throws SQLException{
		List<WebSite> sites = new Vector<WebSite>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			WebSite site = new WebSite();
			site.id = rs.getInt("id");
			site.url = rs.getString("url");				//网址
			site.parser = rs.getString("parser");		//Python解析器
			site.note = rs.getString("note");			//备注
			sites.add(site);
		}
		conn.close();
		return sites;
	}	
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(WebSite site) throws SQLException{
		String sql = "insert into website(url, parser, note)"
			+ " values (?, ?, ?)";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,site.url);				// 网址
		stmt.setString(2,site.parser);			// Python解析器
		stmt.setString(3,site.note);			// 备注
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception{
		WebSite site;
		/*/添加
		site = new WebSite();
		site.url = "http://www.baidu.com";
		site.parser = "BaiduParser";
		site.note = "很好很强大";
		WebSite.persistent(site);
		//*///修改
		site = WebSite.getWebSite(1);
		System.out.println(site.url);
		//*/
		//*列出
		List<WebSite> sites = WebSite.getWebSites();
		for(WebSite one:sites){
			System.out.println(one.url);
		}
		Integer.parseInt("hi");
		//*/
	}

}
