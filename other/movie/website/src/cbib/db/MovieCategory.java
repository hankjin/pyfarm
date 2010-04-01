package cbib.db;
import java.sql.*;
import java.util.List;
import java.util.Vector;

/**
*@hankjin
*@电影分类v1.0
*
*/
public class MovieCategory
{
	public int id;
	public String name;		// 分类名称
	public int parent;		// 父类
	/**
	*创建对象
	*默认全部为空
	*/
	public MovieCategory(){
		this.id = 1;
		this.name = "";
		this.parent = 1;
	}
	/**
	 * 更新数据库
	 * @return
	 * @throws SQLException
	 */
	public boolean update2DB()throws SQLException{
		String sql = "update movieCategory set name=?, parent=? "
			+ " where id=?";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,this.name);			// 分类名称
		stmt.setInt(2,this.parent);				// 父类
		stmt.setInt(3,this.id);					// ID
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	 * 根据ID删除分类
	 * @param id
	 * @return
	 * @throws SQLException
	 */
	public static boolean delCategory(int id) throws SQLException{
		DBConn conn = new DBConn();
		Statement stmt = conn.getStmt();
		int result = stmt.executeUpdate("delete from movieCategory where id="+id);
		conn.close();
		return result > 0;		
	}
	/**
	*根据ID获取对象
	*/
	public static MovieCategory getMovieCategory(int id) throws SQLException{
		List<MovieCategory> result = getMovieCategorys("Select * from movieCategory where id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	/**
	 * 获取所有电影分类
	 * @return
	 * @throws SQLException
	 */
	public static List<MovieCategory> getMovieCategorys() throws SQLException{
		return getMovieCategorys("select * from movieCategory");
	}
	/**
	*根据SQL获取对象
	*/	
	public static List<MovieCategory> getMovieCategorys(String sql) throws SQLException{
		List<MovieCategory> movieCategorys = new Vector<MovieCategory>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			MovieCategory category = new MovieCategory();
			category.id = rs.getInt("id");
			category.name = rs.getString("name");
			category.parent = rs.getInt("parent");
			movieCategorys.add(category);
		}
		conn.close();
		return movieCategorys;
	}	
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(MovieCategory movieCategory) throws SQLException{
		String sql = "insert into movieCategory(name, parent)"
			+ " values (?, ?)";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1, movieCategory.name);			//电影分类名称      
		stmt.setInt(2, movieCategory.parent);			//父类                   
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	 * 测试
	 * @param args
	 */
	public static void main(String args[]) throws Exception{
		MovieCategory mc;
		/*/
		mc = new MovieCategory();
		mc.name = "未分类";
		MovieCategory.persistent(mc);
		//*/
		mc = MovieCategory.getMovieCategory(1);
		System.out.println(mc.name);
	}
}