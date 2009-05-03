<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

<xsl:template match="/">
<html>
<head>
<style>
	body
	{
	font-family: "arnold 2.1"; font-size: 10pt; color: #fff;
	background-image:url(bg.jpg);
	}
	.title
	{
	display: inline-block; font-size: 24pt; letter-spacing: 2px;
	}
	.version { display: inline-block; font-size: 8pt; }
	.author div { font-family: "verdana"; display: inline-block; }
	.url { font-family: "verdana"; }
	.icon { position: absolute; left: 300px; top: 60px }
</style>
</head>
<body>
<div>
	<xsl:apply-templates select="widget/icon"/>
	<div class="title">
		<xsl:value-of select="widget/name"/>
	</div>
	<div class="version"><xsl:apply-templates select="widget/@version"/></div>
</div>
	
<xsl:apply-templates select="widget/description"/>
<div class="url">
<xsl:apply-templates select="widget/author/@href"/>
</div>
	
<div class="author">
<xsl:apply-templates select="widget/author"/>
<xsl:apply-templates select="widget/author/@email"/>
</div>
</body>
</html>
</xsl:template>

<xsl:template match="widget/icon">
<img class="icon" width="75px" height="75px">
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
<p><xsl:value-of select="."/></p>
</xsl:template>

</xsl:stylesheet>
