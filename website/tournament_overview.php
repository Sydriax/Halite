<html lang="en">
<head>
    <?php include 'includes/header.php'; ?>

    <title><?php echo ucwords(str_replace('_', ' ', basename(__FILE__, '.php'))); ?></title>

    <?php include 'includes/prism_styles.php'; ?>
    <link href="lib/bootstrap.min.css" rel="stylesheet">
    <link href="style/general.css" rel="stylesheet">
    <link href="style/learn.css" rel="stylesheet">
</head>
<body>
<div class="container">
    <?php include 'includes/navbar.php'; ?>
    <div class="row">
        <?php include 'includes/learn_sidebar.php'; ?>
        <div class="col-sm-9">
            <h1><?php echo ucwords(str_replace('_', ' ', basename(__FILE__, '.php'))); ?></h1>
            <?php echo $Parsedown->text(file_get_contents(__DIR__ . "/learn/tournament/about_the_tournament.md")); ?>
        </div>
    </div>
    <?php include 'includes/footer.php'; ?>
</div>


<script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.4/jquery.min.js"></script>
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
<?php include 'includes/prism_scripts.php'; ?>
<script src="script/backend.js"></script>
<script src="script/general.js"></script>
</body>
</html>
