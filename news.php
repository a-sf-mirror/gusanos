<!-- insert fancy DOCTYPE here -->
<html>
<head>
	<title>GUSANOS - a liero clone</title>
	<link rel="stylesheet" href="style/style.css"/>
	<!--[if lt IE 7]>
		<link rel="stylesheet" href="style/ie.css"/>
	<![endif]-->
	<script>
		onload=function()
		{
			aa = document.getElementById('menu').getElementsByTagName('a')
			for (var i=0; i<aa.length; i++) if (aa[i].href==location) {
				aa[i].className = 'current'
				break
			}
		}
	</script>
</head>
<body>
	<div id="logo">
		<a href="home.php"><img src="img/logohover.png"/></a>
	</div>
	
	<div id="menu">
		<a href="home.php">home</a>
		<a href="news.php">news</a>
		<a href="http://sourceforge.net/projects/gusanos">get</a>
		<a href="documentation/doku/doku.php?id=main:main">docs</a>
		<a href="http://comser.liero.org.pl/forum">forum</a>
		<a href="http://www.cafepress.com/gusanos">shop</a>
	</div>
	
	<div id="links">
		<a href="http://www.boost.org"><img src="img/boost.png"/></a>
		<a href="http://www.talula.demon.co.uk/allegro"><img src="img/allegro.png"/></a>
		<a href="http://www.fmod.org"><img src="img/fmod.png"/></a>
		<a href="http://www.zoidcom.com"><img src="img/zoidcom.png"/></a>
		<a href="http://www.lua.org"><img src="img/lua.png"/></a>
		<a href="http://www.sf.net"><img src="img/sf.png"/></a>
		<a href="news.php#" class="liero"><img src="img/liero.png"/></a>	</div>
	
	<div id="content">
		<h1>News</h1>
		
		<script src="http://comser.liero.org.pl/guscomser.php"></script>
		
		<div style="text-align: center;"><a href="http://comser.liero.org.pl/forum/index.php?board=7.0">news archive</a></div>
	</div>
	<div id="footer">
		this <u>invalid</u> soup of (X)HTML by pilaf
		<!--[if IE]>.·o IE blows!<![endif]-->
	</div>
</body>
</html>