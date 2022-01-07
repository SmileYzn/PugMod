<?php
include("pre.php");
//
$auth        = filter_input(INPUT_GET, 'auth', FILTER_SANITIZE_STRING);
$player      = (new Player)->get($auth);
$weaponStats = (new WeaponStats)->getStats("player.auth = '{$auth}'");
?>
<?php if(!empty($weaponStats)): ?>
<div class="row">
    <div class="col-md-12">
        <table class="table table-sm table-stripped table-bordered text-center table-dark small">
            <tr>
                <th>Weapon</th>
                <th><abbr title="Frags">F</abbr></th>
                <th><abbr title="Deaths">D</abbr></th>
                <th><abbr title="Headshots">HS</abbr></th>
                <th><abbr title="Headshot Percentage">HSP</abbr></th>
            </tr>
            <?php foreach($weaponStats as $row): ?>
            <tr>
                <td><?= ucfirst($row['weapon']) ?></td>
                <td><?= $row['kills'] ?></td>
                <td><?= $row['death'] ?></td>
                <td><?= $row['hs'] ?></td>
                <td><?= sprintf("%.3f",$row['hsp']) ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</div>
<?php endif; ?>
<?php
include("pro.php");