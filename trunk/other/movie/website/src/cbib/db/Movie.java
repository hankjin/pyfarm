package cbib.db;
import java.sql.*;
import java.util.List;
import java.util.Vector;

/**
*@hankjin
*@电影v1.0
*@todo
*/
public class Movie
{		
	public int id;
	/** 电影名 */
	public String name;			
	/** 类别 */
	public String category;		
	/** 导演 */		
	public String director;
	/** 主演 */
	public String actor;		
	/** 描述 */
	public String description;
	/** 剧照 */
	public String poster;	
	/** 预告片 */
	public String preview;	
	/** 在线观看 */
	public String online;	
	/**
	*创建对象
	*默认全部为空
	*/
	public Movie(){
		id = 1;
		name = "";			// 电影名
		category = "";		// 类别
		director = "";
		actor = "";		
		description = "";	// 描述
		poster = "";		// 剧照
		preview = "";		// 预告片
		online = "";		// 在线观看
	}
	/**
	*根据ID获取对象
	*/
	public static Movie getMovie(int id) throws SQLException{
		List<Movie> result = getMovies("Select * from movie where id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	/**
	*根据分类获取电影
	*/
	public static List<Movie> getMoviesOfCategory(int category) throws SQLException{
		return getMovies("Select * from movie where category like '%"+category+"%'");
	}
	/**
	*根据分类获取电影
	*/
	public static List<Movie> getMoviesOfCategory(String category) throws SQLException{
		return getMovies("Select * from movie where category like '%"+category+"%'");
	}
	
	/**
	*根据SQL获取对象
	*/	
	public static List<Movie> getMovies(String sql) throws SQLException{
		List<Movie> movies = new Vector<Movie>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			Movie movie = new Movie();
			movie.id = rs.getInt("id");
			movie.name = rs.getString("name");					// 电影名
			movie.director = rs.getString("director");
			movie.actor = rs.getString("actor");
			movie.category = rs.getString("category");			// 类别
			movie.description = rs.getString("description");	// 描述
			movie.poster = rs.getString("poster");				// 剧照
			movie.preview = rs.getString("preview");			// 预告片
			movie.online = rs.getString("online");				// 在线观看
			movies.add(movie);
		}
		conn.close();
		return movies;
	}	
	public static void main(String args[]) throws SQLException{
		//
		List<Movie> movies = Movie.getMoviesOfCategory(Integer.parseInt("5"));
		for(Movie movie:movies){
			System.out.println(movie.name);
		}
	}
}