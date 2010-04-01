package cbib.db;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

/**
*@hankjin
*@数据库连接管理类
*@完成
*/
public class DBConn {
	private static Connection con;
	private Statement stmt;
	private ResultSet rs;
	private static String drivername = "com.mysql.jdbc.Driver";
	private static String url = "jdbc:mysql://movie.cnkk.org:3306/movie?useUnicode=true&characterEncoding=UTF-8";
	private static String user = "movie";
	private static String pass = "eivom123";
	public static String test = "Hello";//if properties was loaded successfully, it should be Worlp
	static{
		try{
			Properties props = new Properties(); 
			props.load(cbib.db.DBConn.class.getClassLoader().getResourceAsStream("jdbc.properties"));
			url = props.getProperty("url");
			user = props.getProperty("user");
			pass = props.getProperty("pass");
			test = props.getProperty("test");
			
		}
		catch(Exception ioe){
			System.out.println("IO Error when load jdbc.properties");
		}
	}
	/**
	*获取数据库连接
	*/
	public static synchronized Connection getConn() throws Exception {
		try {
			Class.forName(drivername);
			con = DriverManager.getConnection(url, user, pass);
			return con;
		} catch (SQLException e) {
			System.err.println(e.getMessage());
			throw e;
		}
	}
	
	/**
	*获取用于读的Statement
	*/
	public Statement getStmtread() {
		try {
			con = getConn();
			stmt = con.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE,
				ResultSet.CONCUR_READ_ONLY);
			return stmt;
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		return null;
	}
	
	/**
	*根据SQL获取结果集
	*/
	public ResultSet getRs(String sql) throws SQLException{
		stmt = getStmtread();
		rs = stmt.executeQuery(sql);
		return rs;
	}
	/**
	*根据SQL获取PrepareStatement
	*/
	public PreparedStatement getStmt(String sql) {
		try {
			con = getConn();
			stmt = con.prepareStatement(sql);
			return (PreparedStatement)stmt;
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		return null;
	}

	/**
	*获取Statement
	*/
	public Statement getStmt() {
		try {
			con = getConn();
			stmt = con.createStatement();
			return stmt;
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		return null;
	}
	
	/**
	*关闭连接
	*/
	public synchronized void close() {
		try {
			if (rs != null) {
				rs.close();
				rs = null;
			}
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		try {
			if (stmt != null) {
				stmt.close();
				stmt = null;
			}
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		try {
			if (con != null) {
				con.close();
				con = null;
			}
		} catch (Exception e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
	}
	/**
	*@测试方法
	*/
	public static void main(String args[]) throws Exception{
		//*/
		DBConn conn = new DBConn();
		Statement stmt = conn.getStmt();
		stmt.executeUpdate("insert into test(name, pass) values('你好','123')");
		ResultSet rs = new DBConn().getRs("select * from test");
		while(rs.next()){
			String name = rs.getString("name");
			System.out.println(name);
		}
		//*/
		
	}

}