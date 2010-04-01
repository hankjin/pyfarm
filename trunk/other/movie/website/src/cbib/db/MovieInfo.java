package cbib.db;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;
import java.util.Vector;

import cbib.db.filter.MFilter;

/**
 * 影讯,核心资讯
 * @author hankjin
 *
 */
public class MovieInfo {
	public int id;
	public String hdate;		// 放映日期
	public String htime;		// 放映时间
	public Cinema cinema;		// 影院
	public Movie movie;		// 电影
	public int price;			// 价格
	public String poster;		// 海豹
	public MovieInfo(){
		id=1;
		hdate = "";
		cinema = new Cinema();
		movie = new Movie();
		price = 0;
		poster = "";
	}
	/**
	 * 根据电影编号获取信息
	 * @param movieid
	 * @return
	 */
	public static List<MovieInfo> getMovieInfoByMovie(int movieid){
		List<MovieInfo> infos = new Vector<MovieInfo>();
		return infos;
	}
	/**
	*根据ID获取对象
	*/
	public static MovieInfo getMovieInfo(int id) throws SQLException{
		List<MovieInfo> result = getMovieInfos("select * from movieinfo, movie, cinema where movieinfo.cinema=cinema.id and movieinfo.movie=movie.id and movieinfo.id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	public static List<MovieInfo> getMovieInfos(MFilter filter) throws SQLException{
		return getMovieInfos("select * from movieinfo, movie, cinema where movieinfo.cinema=cinema.id and movieinfo.movie=movie.id "+filter.toString());
	}
	/**
	 * 获取所有影讯
	 * @return
	 * @throws SQLException
	 */
	public static List<MovieInfo> getMovieInfos() throws SQLException{
		return getMovieInfos("select * from movieinfo, movie, cinema where movieinfo.cinema=cinema.id and movieinfo.movie=movie.id");
	}
	/**
	*根据SQL获取对象
	*/	
	public static List<MovieInfo> getMovieInfos(String sql) throws SQLException{
		List<MovieInfo> infos = new Vector<MovieInfo>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			MovieInfo info = new MovieInfo();
			info.id = rs.getInt("movieinfo.id");
			info.hdate = rs.getString("movieinfo.hdate");
			info.htime = rs.getString("movieinfo.htime");
			info.cinema = new Cinema();
			info.cinema.id = rs.getInt("cinema.id");
			info.cinema.name = rs.getString("cinema.name");
			info.cinema.introduce = rs.getString("cinema.introduce");
			info.movie = new Movie();
			info.movie.id = rs.getInt("movie.id");
			info.movie.name = rs.getString("movie.name");
			info.movie.poster = rs.getString("movie.poster");
			info.movie.description = rs.getString("movie.description");
			infos.add(info);
		}
		conn.close();
		return infos;
	}	
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(Cinema cinema) throws SQLException{
		return true;
	}
	public String toString(){
		return hdate + htime + cinema.name + movie.name;
	}
	public static void main(String args[])throws SQLException{
		MFilter filter = new MFilter();
		//filter.addCondition("cinema.name", "武昌");
		List<MovieInfo> infos = MovieInfo.getMovieInfos(filter);
		for(MovieInfo info:infos){
			System.out.println(info.id);
		}
		System.out.println(infos.size());
	}
}
