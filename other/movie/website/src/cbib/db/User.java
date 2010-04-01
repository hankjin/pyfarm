package cbib.db;
import java.sql.Date;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;
import java.util.Vector;

/**
*@hankjin
*@用户v1.0
*/
public class User
{	
	public String id;				// ID
	public String pass;				// 密码
	public String name;				// 姓名
	public String gender;			// 性别
	public Date birthday;			// 生日
	public String hobby;			// 爱好
	public String phone;			// 手机号
	public String email;			// Email
	/**
	*创建对象
	*默认全部为空
	*/
	public User(){
		this.id = "";				// ID
		this.pass = "";				// 密码
		this.name = "";				// 姓名
		this.gender = "";			// 性别
		this.birthday = new Date(0);// 生日
		this.hobby = "";			// 爱好
		this.phone = "";			// 手机号
		this.email = "";			// Email
	}
	/**
	 * 更新个人信息到数据库
	 * @return
	 * @throws SQLException
	 */
	public boolean update2DB()throws SQLException{
		String sql = "update user set pass=?, name=?, gender=?, birthday=?, hobby=?, phone=?, email=?"
			+ " where id=?";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,this.pass);			// 密码
		stmt.setString(2,this.name);			// 姓名
		stmt.setString(3,this.gender);			// 性别
		stmt.setDate(4,this.birthday);			// 生日
		stmt.setString(5,this.hobby);			// 爱好
		stmt.setString(6,this.phone);			// 手机号
		stmt.setString(7,this.email);			// Email
		stmt.setString(8,this.id);				// ID
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	*根据ID获取对象
	*/
	public static User getUser(String id) throws SQLException{
		List<User> result = getUsers("Select * from user where id='"+id+"'");
		return result.size() > 0? result.get(0) : null;
	}
	/**
	*根据SQL获取对象
	*/	
	public static List<User> getUsers(String sql) throws SQLException{
		List<User> users = new Vector<User>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			User user = new User();
			user.id = rs.getString("id");				// ID
			user.pass = rs.getString("pass");			// 密码
			user.name = rs.getString("name");			// 姓名
			user.gender = rs.getString("gender");		// 性别
			user.birthday = rs.getDate("birthday");		// 生日
			user.hobby = rs.getString("hobby");			// 爱好
			user.phone = rs.getString("phone");			// 手机号
			user.email = rs.getString("email");			// Email
			users.add(user);
		}
		conn.close();
		return users;
	}	
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(User user) throws SQLException{
		String sql = "insert into user(id, pass, name, gender, birthday, hobby, phone, email)"
			+ " values (?, ?, ?, ?, ?, ?, ?, ?)";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,user.id);				// ID
		stmt.setString(2,user.pass);			// 密码
		stmt.setString(3,user.name);			// 姓名
		stmt.setString(4,user.gender);			// 性别
		stmt.setDate(5,user.birthday);			// 生日
		stmt.setString(6,user.hobby);			// 爱好
		stmt.setString(7,user.phone);			// 手机号
		stmt.setString(8,user.email);			// Email   
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	public static void main(String args[]) throws Exception{
		User user;
		/*///保存
		user = new User();
		user.id = "hankjin";
		user.pass = "hjz";
		user.name = "黄进东";
		user.gender = "M";
		user.email = "hankjin@163.com";
		user.phone = "13647289143";
		User.persistent(user);
		/*/
		user = new User();
		user.id="test1";
		user.pass = "test";
		System.out.println(User.persistent(user));
		//*///修改
		user = User.getUser("hankjin");
		user.name = "黄进东";
		user.update2DB();
		//*///查询
		user = User.getUser("hankjin");
		System.out.println(user.name);
		//*/
	}
}