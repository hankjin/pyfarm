package cbib.db;
import java.sql.*;
import java.util.List;
import java.util.Vector;

/**
* @hankjin
* @电影院v1.0
* @完成
*/
public class Cinema
{
	public int id;
	public String name;				//电影院名称
	public String address;			//地址
	public String phone;				//电话
	public String workHour;			//营业时间
	public int price;						//一般价格
	public String discount;			//打折
	public String tranffic;				//交通
	public String introduce;			//简介
	public String environment;	 	//影院条件
	/**
	*默认全部为空
	*/
	public Cinema(){
		name = "";					//电影院名称
		address = "";				//地址
		phone = "";				//电话
		workHour = "";			//营业时间
		price = 0;						//一般价格
		discount = "";				//打折
		tranffic = "";				//交通
		introduce = "";			//简介
		environment = "";		//影院条件
	}
	/**
	*根据ID获取Cinema对象
	*/
	public static Cinema getCinema(int id) throws SQLException{
		List<Cinema> result = getCinemas("Select * from cinema where id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	/**
	 * 获取所有影院列表
	 * @return
	 * @throws SQLException
	 */
	public static List<Cinema> getCinemas() throws SQLException{
		return getCinemas("select * from cinema");
	}
	/**
	 * 根据SQL语句获取影院列表
	 * @param sql
	 * @return
	 * @throws SQLException
	 */
	public static List<Cinema> getCinemas(String sql) throws SQLException{
		List<Cinema> cinemas = new Vector<Cinema>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			Cinema cinema = new Cinema();
			cinema.id = rs.getInt("id");	
			cinema.name = rs.getString("name");								//电影院名称
			cinema.address = rs.getString("address");						//地址
			cinema.phone = rs.getString("phone");							//电话
			cinema.workHour = rs.getString("workHour");				//营业时间
			cinema.price = rs.getInt("price");									//一般价格
			cinema.discount = rs.getString("discount");					//打折
			cinema.tranffic = rs.getString("tranffic");						//交通
			cinema.introduce = rs.getString("introduce");					//简介
			cinema.environment = rs.getString("environment");		//影院条件
			cinemas.add(cinema);
		}
		conn.close();
		return cinemas;
	}
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(Cinema cinema) throws SQLException{
		String sql = "insert into cinema(name, address, phone, workHour, price, discount, tranffic, introduce, environment)"
			+ " values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1, cinema.name);						//电影院名称      
		stmt.setString(2, cinema.address);					//地址                   
		stmt.setString(3, cinema.phone);						//电话                   
		stmt.setString(4, cinema.workHour);				//营业时间           
		stmt.setInt(5, cinema.price);								//一般价格           
		stmt.setString(6, cinema.discount);					//打折                   
		stmt.setString(7, cinema.tranffic);						//交通                   
		stmt.setString(8, cinema.introduce);					//简介                   
		stmt.setString(9, cinema.environment);			//影院条件           
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	*测试方法
	*/
	public static void main(String args[]) throws Exception{
		Cinema cinema;
		//*
		cinema = new Cinema();
		cinema.name = new String("亚贸兴汇影城".getBytes(),"UTF-8");
		cinema.address = new String("武昌武珞路628号武商亚贸6楼".getBytes(),"UTF-8");
		cinema.phone = "027-87665866 87854279";
		Cinema.persistent(cinema);
		//*/
		//*/
		cinema = Cinema.getCinema(1);
		System.out.println("Hi"+cinema.name);
		//*/
	}
	
}