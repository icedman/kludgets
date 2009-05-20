<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

<xsl:template match="/">
<html>
<head>
<style>
	* { font-family: "verdana"; font-size: 10pt; color: #000; }
	body { background-image:url(bg.png); }
	.title
	{
		font-family: "arnold 2.1"; 
		width:400px;
		height:40px;
		overflow:hidden;
		font-size: 24pt; color: #fff;
	}
	.titleGradient{
		position:absolute;
		top:0px;
		left:0px;
		height:50px;
		width:400px;
		background:url(titlegradient.png) repeat-x;
	}
	.info
	{
		position: absolute;
		top: 50px;
		margin: 10px;
	}
	.version { display: inline-block; font-size: 8pt; }
	.author div { display: inline-block; }
	.icon { padding-right: 20px; }
</style>
</head>
<body>

<div class="title">
	<xsl:value-of select="widget/name"/>
</div>
<div class="titleGradient"></div>

<div class="info">

<xsl:apply-templates select="widget/icon"/>

<!--
<div class="version">
<xsl:apply-templates select="widget/@version"/>
</div>
-->
	
<xsl:apply-templates select="widget/description"/>

<!--
<div class="url">
	<a href="http://www.google.com" target="new">
		<xsl:attribute name="src">
		<xsl:value-of select="widget/author/@href"/>
		</xsl:attribute>
		Website
	</a>
</div>

<div class="author">
<xsl:apply-templates select="widget/author"/>
<xsl:apply-templates select="widget/author/@email"/>
</div>
-->
	
</div>
	
</body>
</html>
</xsl:template>

<xsl:template match="widget/icon">
<img class="icon" width="64px" height="64px" align="left">
<xsl:attribute name="src">
<xsl:value-of select="."/>
</xsl:attribute>
</img>
</xsl:template>

<xsl:template match="widget/@version">
<span>version:<xsl:value-of select="."/></span>
</xsl:template>

<xsl:template match="widget/author">
<div><xsl:value-of select="."/></div>
</xsl:template>

<xsl:template match="widget/author/@email">
<div><xsl:value-of select="."/></div>
</xsl:template>

<xsl:template match="widget/author/@href">
<div><xsl:value-of select="."/></div>
</xsl:template>

<xsl:template match="widget/description">
<span><xsl:value-of select="."/></span>
</xsl:template>

</xsl:stylesheet>
