package cbib.db;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;
import java.util.Vector;

/**
*@hankjin
*@电影v1.0
*@todo
*/
public class Comment
{
	public int id;
	/** 电影ID */
	public int movie;	
	/** 评论用户 */
	public String user;
	/** 评论标题 */
	public String title;
	/** 评论内容 */
	public String comment;
	/** 评论时间 */
	public String hwhen;
	
	/**
	*创建对象
	*默认全部为空
	*/
	public Comment(){
		id = 0;
		movie = 1;
		user = "匿名";
		title = "无";
		comment = "";
		hwhen = "";
	}
	/**
	*根据ID获取对象
	*/
	public static Comment getComment(int id) throws SQLException{
		List<Comment> result = getComments("Select * from comment where id="+id);
		return result.size() > 0? result.get(0) : null;
	}

	/**
	*根据电影ID获取评论
	*/
	public static List<Comment> getCommentsOfMovie(int movie) throws SQLException{
		return getComments("Select * from comment where movie ="+movie);
	}
	
	/**
	*根据SQL获取对象
	*/	
	public static List<Comment> getComments(String sql) throws SQLException{
		List<Comment> comments = new Vector<Comment>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			Comment c = new Comment();
			c.id = rs.getInt("id");
			c.movie = rs.getInt("movie");
			c.user = rs.getString("user");
			c.title = rs.getString("title");
			c.comment = rs.getString("comment");
			c.hwhen = rs.getString("hwhen");
			comments.add(c);
		}
		conn.close();
		return comments;
	}	
	public static void main(String args[]) throws SQLException{
		//
		List<Comment> comments = Comment.getCommentsOfMovie(76187);
		for(Comment comment:comments){
			System.out.println(comment.title);
		}
	}
}