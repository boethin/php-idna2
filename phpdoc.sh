rm -rf phpdoc && mkdir phpdoc && phpdoc -t phpdoc -f idna2.php --title php-idna2 --template responsive

echo $(cat <<'EOF'
.navbar-fixed-top { display:none; }
body { padding-top:0; }
EOF
) >phpdoc/hack.css
perl -pi -e 's|</head>|<link rel="stylesheet" href="../hack.css"></head>|' phpdoc/packages/Default.html

