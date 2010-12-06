<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

<xsl:template match="/">
<widget>
<xsl:apply-templates select="plist/dict/key[text()='CFBundleIdentifier']"/>
<xsl:apply-templates select="plist/dict/key[text()='CFBundleName']"/>
<!--<xsl:apply-templates select="plist/dict/key[text()='CFBundleDisplayName']"/>-->
<xsl:apply-templates select="plist/dict/key[text()='CFBundleVersion']"/>
<xsl:apply-templates select="plist/dict/key[text()='Width']"/>
<xsl:apply-templates select="plist/dict/key[text()='Height']"/>
<xsl:apply-templates select="plist/dict/key[text()='MainHTML']"/>
<xsl:apply-templates select="plist/dict/key[text()='Description']"/>
<access>
<xsl:apply-templates select="plist/dict/key[text()='AllowFileAccess-OutsideOfWidget']"/>
<xsl:apply-templates select="plist/dict/key[text()='AllowFullAccess']"/>
<xsl:apply-templates select="plist/dict/key[text()='AllowInternetPlugins']"/>
<xsl:apply-templates select="plist/dict/key[text()='AllowNetworkAccess']"/>
<xsl:apply-templates select="plist/dict/key[text()='AllowSystem']"/>
</access>
<author>
<xsl:apply-templates select="plist/dict/key[text()='Author']"/>
<xsl:apply-templates select="plist/dict/key[text()='AuthorMail']"/>
<xsl:apply-templates select="plist/dict/key[text()='AuthorUrl']"/>
</author>
<plugins>
<xsl:apply-templates select="plist/dict/key[text()='Plugin']"/>
</plugins>
</widget>
</xsl:template>

<xsl:template match="key[text()='CFBundleIdentifier']">
<id><xsl:value-of select="following-sibling::*[1]"/></id>
</xsl:template>

<xsl:template match="key[text()='CFBundleName']">
<name><xsl:value-of select="following-sibling::*[1]"/></name>
</xsl:template>

<xsl:template match="key[text()='CFBundleDisplayName']">
<name><xsl:value-of select="following-sibling::*[1]"/></name>
</xsl:template>

<xsl:template match="key[text()='CFBundleVersion']">
<version><xsl:value-of select="following-sibling::*[1]"/></version>
</xsl:template>

<xsl:template match="key[text()='Width']">
<width><xsl:value-of select="following-sibling::*[1]"/></width>
</xsl:template>

<xsl:template match="key[text()='Height']">
<height><xsl:value-of select="following-sibling::*[1]"/></height>
</xsl:template>

<xsl:template match="key[text()='MainHTML']">
<content><src><xsl:value-of select="following-sibling::*[1]"/></src></content>
</xsl:template>

<xsl:template match="key[text()='Description']">
<description><xsl:value-of select="following-sibling::*[1]"/></description>
</xsl:template>

<xsl:template match="key[text()='AllowFileAccess-OutsideOfWidget']">
<local><xsl:apply-templates select="following-sibling::*[1]"/></local>
</xsl:template>

<xsl:template match="key[text()='AllowFullAccess']">
<full><xsl:apply-templates select="following-sibling::*[1]"/></full>
</xsl:template>

<xsl:template match="key[text()='AllowInternetPlugins']">
<plugins><xsl:apply-templates select="following-sibling::*[1]"/></plugins>
</xsl:template>

<xsl:template match="key[text()='AllowNetworkAccess']">
<network><xsl:apply-templates select="following-sibling::*[1]"/></network>
</xsl:template>

<xsl:template match="key[text()='AllowSystem']">
<system><xsl:apply-templates select="following-sibling::*[1]"/></system>
</xsl:template>

<xsl:template match="key[text()='Plugin']">
<plugin><xsl:apply-templates select="following-sibling::*[1]"/></plugin>
</xsl:template>

<xsl:template match="key[text()='Author']">
<name><xsl:apply-templates select="following-sibling::*[1]"/></name>
</xsl:template>

<xsl:template match="key[text()='AuthorMail']">
<email><xsl:apply-templates select="following-sibling::*[1]"/></email>
</xsl:template>

<xsl:template match="key[text()='AuthorUrl']">
<href><xsl:apply-templates select="following-sibling::*[1]"/></href>
</xsl:template>

<xsl:template match="true">true</xsl:template>
<xsl:template match="false">false</xsl:template>

<!--
AllowJava
AllowMultipleInstances
-->

</xsl:stylesheet>
