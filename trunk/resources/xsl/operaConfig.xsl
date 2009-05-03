<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

<xsl:template match="/">
<widget>
<name><xsl:value-of select="widget/widgetname"/></name>
<description><xsl:value-of select="widget/description"/></description>
<width><xsl:value-of select="widget/width"/></width>
<height><xsl:value-of select="widget/height"/></height>
<icon><xsl:value-of select="widget/icon"/></icon>
<id><xsl:value-of select="widget/id/host"/>-<xsl:value-of select="widget/id/name"/></id>
<access><network>true</network></access>
<author>
<xsl:apply-templates select="widget/author/name"/>
<xsl:apply-templates select="widget/author/email"/>
<xsl:apply-templates select="widget/author/link"/>
</author>
</widget>
</xsl:template>

<xsl:template match="widget/author/name">
<name><xsl:value-of select="."/></name>
</xsl:template>

<xsl:template match="widget/author/email">
<email><xsl:value-of select="."/></email>
</xsl:template>

<xsl:template match="widget/author/link">
<href><xsl:value-of select="."/></href>
</xsl:template>

</xsl:stylesheet>
