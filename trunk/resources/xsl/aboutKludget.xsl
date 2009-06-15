<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

<xsl:template match="/">
<html>
<head>
<style>
	* { font-family: "verdana"; font-size: 10pt; color: #000; }
	body { background-image:url(bgBlack.png); }
	.title
	{
	width:400px;
	height:40px;
	}
	.info
	{
	position: absolute;
	top: 80px;
	margin: 10px;
	width: 380px;
	}
	.version { position: absolute; left:10px; top: 44px;
				font-family: "arnold 2.1"; font-size: 8pt; color: #fff; }
	.author div { display: inline-block; }
	.icon { padding-left: 10px; }
</style>
</head>
<body>

<div class="title">
<img>
<xsl:attribute name="src">
<xsl:value-of select="widget/logo"/>
</xsl:attribute>
</img>
</div>

<div class="version">
<xsl:value-of select="widget/version"/>
</div>
	
<div class="info">

<img class="icon" align="right">
<xsl:attribute name="src">
<xsl:value-of select="widget/icon"/>
</xsl:attribute>
</img>
	
<p>
Kludgets is an opensource project released under the GNU Lesser General Public License.
</p>
<p>
Copyright 2008-2009 MindHyve Inc.
</p>

</div>
	
</body>
</html>
</xsl:template>

</xsl:stylesheet>
