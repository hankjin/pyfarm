package cbib.db;
import java.sql.*;
import java.util.List;
import java.util.Vector;

/**
*@hankjin
*@数据库工具类
*@todo
*/
public class DBUtil
{		
	/**
	*根据SQL获取对象
	*/	
	public static List<String> getMovieDate() throws SQLException{
		List<String> result = new Vector<String>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs("Select distinct hdate from movieinfo");
		while(rs.next()){
			result.add(rs.getString("hdate"));
		}
		conn.close();
		return result;
	}	
	public static void main(String args[]) throws SQLException{
		//
		List<Movie> movies = Movie.getMoviesOfCategory(Integer.parseInt("5"));
		for(Movie movie:movies){
			System.out.println(movie.name);
		}
	}
}